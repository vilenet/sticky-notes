#include "app.h"
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/filename.H>

#include <cstring>
#include <errno.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <tuple>

namespace fs = std::filesystem;

StickyNoteApp::StickyNoteApp() : text_changed(false) {
    window = new Fl_Double_Window(300, 350, "Sticky Notes");

    text_buffer = new Fl_Text_Buffer();
    text_editor = new Fl_Text_Editor(0, 25, 300, 325);
    text_editor->buffer(text_buffer);
    text_editor->box(FL_NO_BOX);
    Fl_Color editor_color = fl_rgb_color(255, 248, 209); //Default color #FFF8D1
    text_editor->color(editor_color);

    Fl_Color menu_color = fl_rgb_color(255, 242, 171); //Default color #FFF2AB
    menu_bar = new Fl_Menu_Bar(0, 0, 300, 25);
    menu_bar->color(menu_color);
    menu_bar->box(FL_FLAT_BOX);

    menu_bar->add("File/New",        FL_COMMAND + 'n', menu_new_callback,     this);
    menu_bar->add("File/Open",       FL_COMMAND + 'o', menu_open_callback,    this, FL_MENU_DIVIDER);
    menu_bar->add("File/Save",       FL_COMMAND + 's', menu_save_callback,    this);
    menu_bar->add("File/Save As...", FL_COMMAND + 'S', menu_save_as_callback, this, FL_MENU_DIVIDER);
    menu_bar->add("File/Quit",       FL_COMMAND + 'q', menu_quit_callback,    this);
    
    window->resizable(text_editor);
    window->end();
}

void StickyNoteApp::run(int argc, char **argv) {
    window->show(argc, argv);
    Fl::run();
}

void StickyNoteApp::set_filename(const char *new_filename) {
    if (new_filename) {
        strncpy(filename, new_filename, sizeof(filename) - 1);
        filename[sizeof(filename) - 1] = '\0';
    } else {
        filename[0] = '\0';
    }
    update_title();
}

const char* StickyNoteApp::get_filename() const { return filename; }

void StickyNoteApp::set_changed(bool changed) {
    text_changed = changed;
    update_title();
}

bool StickyNoteApp::is_text_changed() const { return text_changed; }

void StickyNoteApp::update_title() {
    const char *fname = filename[0] ? fl_filename_name(filename) : "Sticky Notes";
    char buf[FL_PATH_MAX + 3];
    snprintf(buf, sizeof(buf), "%s%s", fname, text_changed ? " *" : "");
    window->copy_label(buf);
}

void StickyNoteApp::menu_new_callback(Fl_Widget*, void *v) {
    StickyNoteApp *app = static_cast<StickyNoteApp*>(v);
    app->text_buffer->text("");
    app->set_changed(false);
}

void StickyNoteApp::menu_open_callback(Fl_Widget*, void *v) {
    StickyNoteApp *app = static_cast<StickyNoteApp*>(v);
    if (app->is_text_changed()) {
        int r = fl_choice("The current file has not been saved.\nWould you like to save it now?",
                          "Cancel", "Save", "Don't Save");
        if (r == 0) return;
        if (r == 1) menu_save_callback(nullptr, v);
    }
    
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Open File...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);

    if (file_chooser.show() == 0) {
        if (app->text_buffer->loadfile(file_chooser.filename()) == 0) {
            app->set_filename(file_chooser.filename());
            app->set_changed(false);
            app->load_property();
            app->window->resize(app->window_x, app->window_y, app->window_width, app->window_height);
            app->window->redraw();
        } else {
            fl_alert("Failed to load file\n%s\n%s", file_chooser.filename(), strerror(errno));
        }
    }
}

void StickyNoteApp::menu_save_callback(Fl_Widget*, void *v) {
    StickyNoteApp *app = static_cast<StickyNoteApp*>(v);
    if (app->get_filename()[0]) {
        app->text_buffer->savefile(app->get_filename());
        app->set_changed(false);
        app->save_property();
    } else {
        menu_save_as_callback(nullptr, v);
    }
}

void StickyNoteApp::menu_save_as_callback(Fl_Widget*, void *v) {
    StickyNoteApp *app = static_cast<StickyNoteApp*>(v);
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Save File As...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

    if (file_chooser.show() == 0) {
        app->set_filename(file_chooser.filename());
        app->text_buffer->savefile(file_chooser.filename());
        app->set_changed(false);
        app->save_property();
    }
}

void StickyNoteApp::menu_quit_callback(Fl_Widget*, void *v) {
    StickyNoteApp *app = static_cast<StickyNoteApp*>(v);
    if (app->is_text_changed()) {
        int choice = fl_choice("The current file has not been saved.\nWould you like to save it now?",
                               "Cancel", "Save", "Don't Save");
        if (choice == 1) menu_save_callback(nullptr, v);
        if (choice == 0) return;
    }
    app->save_property();
    Fl::hide_all_windows();
}

void StickyNoteApp::save_property() {
    int x = window->x();
    int y = window->y();
    int width = window->w();
    int height = window->h();

    fs::path property_dir = fs::current_path() / "data/property";
    fs::path property_file = property_dir / "property.txt";

    if (!fs::exists(property_dir)) {
        fs::create_directories(property_dir);
    }

    std::vector<std::string> lines;
    std::ifstream file_in(property_file);
    if (file_in.is_open()) {
        std::string line;
        while (std::getline(file_in, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string item;
            std::vector<std::string> tokens;
            while (std::getline(ss, item, '|')) {
                tokens.push_back(item);
            }

            if (tokens.size() != 5 || tokens[0] != filename) {
                lines.push_back(line);
            }
        }
        file_in.close();
    }

    std::stringstream new_entry;
    new_entry << filename << "|" << x << "|" << y << "|" << width << "|" << height;

    lines.push_back(new_entry.str());

    std::ofstream file_out(property_file, std::ios::out | std::ios::trunc);
    if (file_out.is_open()) {
        for (const std::string& line : lines) {
            file_out << line << std::endl;
        }
        file_out.close();
    } else {
        fl_alert("Could not open file to save window properties.");
    }
}

void StickyNoteApp::load_property() {
    fs::path property_file = fs::current_path() / "data/property/property.txt";
    if (!fs::exists(property_file)) return;

    std::ifstream file(property_file);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // String format: file_path|x|y|width|height
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string item;
            std::vector<std::string> tokens;

            while (std::getline(ss, item, '|')) {
                tokens.push_back(item);
            }

            if (tokens.size() == 5) {
                if (tokens[0] == filename) {
                    window_x      = std::stoi(tokens[1]);
                    window_y      = std::stoi(tokens[2]);
                    window_width  = std::stoi(tokens[3]);
                    window_height = std::stoi(tokens[4]);
                    break;
                }
            }
        }
        file.close();
    }
}
