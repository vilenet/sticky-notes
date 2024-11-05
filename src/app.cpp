#include "app.h"
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/filename.H>
#include <cstring>
#include <errno.h>

StickyNoteApp::StickyNoteApp() : text_changed(false) {
    window = new Fl_Double_Window(300, 350, "Sticky Notes");
    
    text_buffer = new Fl_Text_Buffer();
    text_editor = new Fl_Text_Editor(0, 25, 300, 325);
    text_editor->buffer(text_buffer);
    
    menu_bar = new Fl_Menu_Bar(0, 0, 300, 25);
    menu_bar->add("File/New",        FL_COMMAND + 'n', menu_new_callback,     this);
    menu_bar->add("File/Open",       FL_COMMAND + 'o', menu_open_callback,    this, FL_MENU_DIVIDER);
    menu_bar->add("File/Save",       FL_COMMAND + 's', menu_save_callback,    this);
    menu_bar->add("File/Save As...", FL_COMMAND + 'S', menu_save_as_callback, this, FL_MENU_DIVIDER);
    menu_bar->add("File/Quit",       FL_COMMAND + 'q', menu_quit_callback,    this);
    
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

void StickyNoteApp::set_changed(bool changed) {
    text_changed = changed;
    update_title();
}

bool StickyNoteApp::is_text_changed() const { return text_changed; }
const char* StickyNoteApp::get_filename() const { return filename; }

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
    Fl::hide_all_windows();
}
