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

StickyNoteApp::StickyNoteApp()
    : Fl_Window(300, 308, "Sticky Notes")
    , editor_color(fl_rgb_color(255, 248, 209))
    , menu_color(fl_rgb_color(255, 242, 171))
    , text_changed(false) 
{
    // Remove standart window tilebar
    this->border(0);

    titlebar.box(FL_FLAT_BOX);
    titlebar.color(menu_color);
    titlebar.label("New Note");

    close_button.color(menu_color);
    close_button.box(FL_FLAT_BOX);
    close_button.labelcolor(fl_rgb_color(80, 80, 80));
    close_button.callback(menu_callback, this);

    items[0].labelsize(16);
    menu_bar.color(menu_color);
    menu_bar.box(FL_FLAT_BOX);
    menu_bar.menu(items.data());
    menu_bar.labelcolor(fl_rgb_color(80, 80, 80));

    text_editor.buffer(text_buffer);
    text_editor.color(editor_color);
    text_editor.box(FL_NO_BOX);

    resizable(text_editor);

    menu_bar.parent(this);
}

StickyNoteApp::~StickyNoteApp() {
    text_editor.buffer(0);
}

void StickyNoteApp::menu_callback(Fl_Widget* sender, void* data) {
    auto menu = dynamic_cast<Fl_Menu_Bar*>(sender);
    auto window = reinterpret_cast<StickyNoteApp*>(data);

    if (sender == &window->close_button) {
        window->menu_quit_action();
        return;
    }

    if (menu && window) {
        const char* label = menu->mvalue()->label();
        if (label) {
            window->handleMenu(label);
        }
    }

    menu->value(0); 
}

void StickyNoteApp::handleMenu(const std::string& label) {
    if      (label == "New")        { menu_new_action(); } 
    else if (label == "Open")       { menu_open_action(); } 
    else if (label == "Save")       { menu_save_action(); } 
    else if (label == "Save as...") { menu_save_as_action(); } 
    else if (label == "Quit")       { menu_quit_action(); }
    else                            { fl_alert(("Unhandled menu action: " + label).c_str()); }
}

// Action methods
void StickyNoteApp::menu_new_action() {
    //v 1
    //text_buffer.text("");
    //set_changed(false);

    //v2
    StickyNoteApp *new_window = new StickyNoteApp();
    new_window->show();
}

void StickyNoteApp::menu_open_action() {
    if (is_text_changed()) {
        int r = fl_choice("The current file has not been saved.\nWould you like to save it now?",
                          "Cancel", "Save", "Don't Save");
        if (r == 0) return;
        if (r == 1) menu_save_action();
    }
    
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Open File...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);

    if (file_chooser.show() == 0) {
        if (text_buffer.loadfile(file_chooser.filename()) == 0) {
            set_filename(file_chooser.filename());
            set_changed(false);

            //load_property();
            //Instead the window_x we can youse x()
            //window->resize(app->window_x, app->window_y, app->window_width, app->window_height);
            //window->redraw();
        }
        else {
            fl_alert("Failed to load file\n%s\n%s", file_chooser.filename(), strerror(errno));
        }
    }
}

void StickyNoteApp::menu_save_action() {
    if (get_filename()[0]) {
        text_buffer.savefile(get_filename());
        set_changed(false);
        //save_property();
    } 
    else {
        menu_save_as_action();
    }
}

void StickyNoteApp::menu_save_as_action() {
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Save File As...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);

    if (file_chooser.show() == 0) {
        set_filename(file_chooser.filename());
        text_buffer.savefile(file_chooser.filename());
        set_changed(false);
        //save_property();
    }
}

void StickyNoteApp::menu_quit_action() {
    if (is_text_changed()) {
        int choice = fl_choice("The current file has not been saved.\nWould you like to save it now?",
                               "Cancel", "Save", "Don't Save");
        if (choice == 1) menu_save_action();
        if (choice == 0) return;
    }
    //save_property();
    Fl::hide_all_windows();
}

// Other methods
void StickyNoteApp::run(int argc, char **argv) {
    show(argc, argv);
    Fl::run();
}

void StickyNoteApp::set_filename(const char *new_filename) {
    if (new_filename) {
        strncpy(filename, new_filename, sizeof(filename) - 1);
        filename[sizeof(filename) - 1] = '\0';
    } 
    else {
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
    titlebar.label(buf);
    titlebar.redraw();
    menu_bar.redraw();
    redraw();
    //copy_label(buf);
}

// property methods
// void StickyNoteApp::save_property() {
//     int x = window->x();
//     int y = window->y();
//     int width = window->w();
//     int height = window->h();

//     fs::path property_dir = fs::current_path() / "data/property";
//     fs::path property_file = property_dir / "property.txt";

//     if (!fs::exists(property_dir)) {
//         fs::create_directories(property_dir);
//     }

//     std::vector<std::string> lines;
//     std::ifstream file_in(property_file);
//     if (file_in.is_open()) {
//         std::string line;
//         while (std::getline(file_in, line)) {
//             if (line.empty()) continue;

//             std::stringstream ss(line);
//             std::string item;
//             std::vector<std::string> tokens;
//             while (std::getline(ss, item, '|')) {
//                 tokens.push_back(item);
//             }

//             if (tokens.size() != 5 || tokens[0] != filename) {
//                 lines.push_back(line);
//             }
//         }
//         file_in.close();
//     }

//     std::stringstream new_entry;
//     new_entry << filename << "|" << x << "|" << y << "|" << width << "|" << height;

//     lines.push_back(new_entry.str());

//     std::ofstream file_out(property_file, std::ios::out | std::ios::trunc);
//     if (file_out.is_open()) {
//         for (const std::string& line : lines) {
//             file_out << line << std::endl;
//         }
//         file_out.close();
//     } else {
//         fl_alert("Could not open file to save window properties.");
//     }
// }

// void StickyNoteApp::load_property() {
//     fs::path property_file = fs::current_path() / "data/property/property.txt";
//     if (!fs::exists(property_file)) return;

//     std::ifstream file(property_file);

//     if (file.is_open()) {
//         std::string line;
//         while (std::getline(file, line)) {
//             // String format: file_path|x|y|width|height
//             if (line.empty()) continue;

//             std::stringstream ss(line);
//             std::string item;
//             std::vector<std::string> tokens;

//             while (std::getline(ss, item, '|')) {
//                 tokens.push_back(item);
//             }

//             if (tokens.size() == 5) {
//                 if (tokens[0] == filename) {
//                     window_x      = std::stoi(tokens[1]);
//                     window_y      = std::stoi(tokens[2]);
//                     window_width  = std::stoi(tokens[3]);
//                     window_height = std::stoi(tokens[4]);
//                     break;
//                 }
//             }
//         }
//         file.close();
//     }
// }

//Fl_Window Redefined methods
int StickyNoteApp::handle(int event) {
    static int click_x = 0, click_y = 0;
    static bool resizing_right = false;
    static bool resizing_bottom = false;
    static bool resizing_left = false;
    static bool resizing_top = false;
    const int resize_border = 10;

    switch (event) {
        case FL_MOVE: {
            int mouse_x = Fl::event_x();
            int mouse_y = Fl::event_y();

            // Изменение курсора для правой границы
            if (mouse_x >= w() - resize_border && mouse_y < h() - resize_border) {
                cursor(FL_CURSOR_WE);
                return 1;
            }

            // Изменение курсора для нижней границы
            if (mouse_y >= h() - resize_border && mouse_x < w() - resize_border) {
                cursor(FL_CURSOR_NS);
                return 1;
            }

            // Изменение курсора для левой границы
            if (mouse_x <= resize_border && mouse_y < h() - resize_border) {
                cursor(FL_CURSOR_WE);
                return 1;
            }

            // Изменение курсора для верхней границы
            if (mouse_y <= resize_border && mouse_x < w() - resize_border) {
                cursor(FL_CURSOR_NS);
                return 1;
            }

            // Сброс курсора, если не на границе
            //cursor(FL_CURSOR_DEFAULT); 
        }
        break;

        case FL_PUSH:
            if (Fl::event_button() == FL_LEFT_MOUSE) {
                click_x = Fl::event_x();
                click_y = Fl::event_y();

                // Проверка на перетаскивание окна по заголовку
                if (Fl::belowmouse() == &titlebar) {
                    return 1;
                }

                // Проверка для правой границы
                if (click_x >= w() - resize_border && click_y < h() - resize_border) {
                    resizing_right = true;
                    return 1;
                }

                // Проверка для нижней границы
                if (click_y >= h() - resize_border && click_x < w() - resize_border) {
                    resizing_bottom = true;
                    return 1;
                }

                // Проверка для левой границы
                if (click_x <= resize_border && click_y < h() - resize_border) {
                    resizing_left = true;
                    return 1;
                }

                // Проверка для верхней границы
                if (click_y <= resize_border && click_x < w() - resize_border) {
                    resizing_top = true;
                    return 1;
                }
            }
            break;

        case FL_DRAG:
            if (Fl::event_state(FL_BUTTON1)) {
                int dx = Fl::event_x() - click_x;
                int dy = Fl::event_y() - click_y;

                // Перетаскивание окна по заголовку
                if (Fl::belowmouse() == &titlebar) {
                    position(x() + dx, y() + dy);
                    return 1;
                }

                // Изменение размера справа
                if (resizing_right) {
                    size(w() + dx, h());
                    click_x = Fl::event_x();
                    close_button.resize(w() - 25, 0, 25, 25);
                    return 1;
                }

                // Изменение размера снизу
                if (resizing_bottom) {
                    size(w(), h() + dy);
                    click_y = Fl::event_y();
                    close_button.resize(w() - 25, 0, 25, 25);
                    return 1;
                }

                //TODO for left and top
            }
            break;

        case FL_RELEASE:
            // Сброс всех флагов изменения размера
            resizing_right = resizing_bottom = resizing_left = resizing_top = false;
            return 1;

        default:
            break;
    }
    return Fl_Window::handle(event);
}

void StickyNoteApp::draw() {
    Fl_Window::draw();

    fl_color(fl_rgb_color(200, 200, 200));
    fl_line_style(FL_SOLID, 1);

    fl_rect(0, 0, w(), h());
}