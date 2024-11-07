#ifndef APP_H
#define APP_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>

#include <cstdio>
#include <string>

class StickyNoteApp {
private:
    Fl_Double_Window *window;
    Fl_Menu_Bar *menu_bar;
    Fl_Text_Editor *text_editor;
    Fl_Text_Buffer *text_buffer;
    bool text_changed;
    char filename[FILENAME_MAX];

private:
    int window_x;
    int window_y;
    int window_width;
    int window_height;

private:
    void update_title();

    static void menu_new_callback    (Fl_Widget*, void *v);
    static void menu_open_callback   (Fl_Widget*, void *v);
    static void menu_save_callback   (Fl_Widget*, void *v);
    static void menu_save_as_callback(Fl_Widget*, void *v);
    static void menu_quit_callback   (Fl_Widget*, void *v);

public:
    StickyNoteApp();

    void run(int argc, char **argv);
    void set_filename(const char *new_filename);
    void set_changed(bool changed);
    bool is_text_changed() const;
    const char* get_filename() const;

    void save_property();
    void load_property();
};

#endif // APP_H
