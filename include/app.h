#ifndef APP_H
#define APP_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>

#include <cstdio>
#include <string>
#include <limits>
#include <vector>
#include <limits>

class StickyNoteApp : public Fl_Window {
private:
    Fl_Box titlebar {0, 0, 300, 25};

    Fl_Menu_Bar menu_bar {0, 0, 25, 25};
    Fl_Button close_button {275, 0, 25, 25, "X"};

    Fl_Text_Editor text_editor {0, 25, 300, 300};
    Fl_Text_Buffer text_buffer;

    Fl_Color editor_color;
    Fl_Color menu_color;

    bool text_changed;
    char filename[FILENAME_MAX];

private:
    // We dont need this, there is w(), h() methods
    // int window_x;
    // int window_y;
    // int window_width;
    // int window_height;

private:
    std::vector<Fl_Menu_Item> items = {
        {"≡", 0, nullptr, nullptr, FL_SUBMENU | FL_ALIGN_CENTER},
        {"New", FL_COMMAND + 'n', menu_callback, this},
        {"Open", FL_COMMAND + 'o', menu_callback, this},
        {"Save", FL_COMMAND + 's', menu_callback, this},
        {"Save As", 0, menu_callback, this, FL_MENU_DIVIDER},
        {"Quit", FL_COMMAND + 'q', menu_callback, this},
        {0}, // Завершающий элемент подменю
        {0}  // Завершающий элемент основного меню
    };

private:
    static void menu_callback(Fl_Widget* sender, void* data);
    void handleMenu(const std::string& label);

    void menu_new_action();
    void menu_open_action();
    void menu_save_action();
    void menu_save_as_action();
    void menu_quit_action();

public:
    StickyNoteApp();
    ~StickyNoteApp();

    void run(int argc, char **argv);
    void update_title();
    void set_filename(const char *new_filename);
    void set_changed(bool changed);
    bool is_text_changed() const;
    const char* get_filename() const;

    //void save_property();
    //void load_property();

public:
    int handle(int event) override;
    void draw() override;
};

#endif // APP_H
