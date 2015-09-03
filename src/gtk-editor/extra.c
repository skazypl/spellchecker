#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "word_list.h"
#include "dictionary.h"

void show_about () {
  GtkWidget *dialog = gtk_about_dialog_new();

  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Text Editor");
  //gtk_window_set_title(GTK_WINDOW(dialog), "About Text Editor");
  
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
     "Text Editor for IPP exercises\n");
    
  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
}

void show_help (void) {
  GtkWidget *help_window;
  GtkWidget *label;
  char help[5000];

  help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (help_window), "Help - Text Editor");
  gtk_window_set_default_size(GTK_WINDOW(help_window), 300, 300);
 
  strcpy(help,
         "\nAby podłączyć usługę spell-checkera do programu trzeba:\n\n"
         "Dołączyć ją do menu 'Spell' w menubar.\n\n"
         "Pobrać zawartość bufora tekstu z edytora: całą lub fragment,\n"
         "  zapamiętując pozycję.\n\n");
  strcat(help, "\0");

  label = gtk_label_new(help);
    
  gtk_container_add(GTK_CONTAINER(help_window), label); 

  gtk_widget_show_all(help_window);
}

// Zaślepeczki słownika (wchar_t i gunichar to prawie to samo)
//
// Oczywiście do zastąpienia prawdziwymi funkcjami

struct old_dictionary {
  int foo;
} old_dict;

struct dictionary* dict = NULL; //chyba nie tutaj
char* dict_location = "slownik.dict";

void update_actual_dict(void)
{
  GtkWidget *dialog;
  FILE* stream;

  if(dict != NULL)
  {
    dictionary_done(dict);
    //free(dict);
  }
  stream = fopen(dict_location, "r");
  if(!stream)
  {
    char* err_msg =
    "Błąd ładowania poprzedniego słownika!\nPlik może być uszkodzony";
    
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_OK,
                                    err_msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }

  dict = dictionary_load(stream);

  if(dict == NULL)
  {
    char* err_msg =
    "Błąd ładowania słownika z pliku!\nPlik może być uszkodzony";
    
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_OK,
                                    err_msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }
  printf("zaladowano slownik %s\n", dict_location);
  printf("%i\n", dictionary_find(dict, L"litwa"));
  fclose(stream);
  // na koniec funkcji, dict jest NULLem albo wskaznikiem na poprawny slownik
  // TODO: gdzieś go niszczyć na wyjściu z programu
}

gboolean old_dictionary_find(const struct dictionary *dict, const wchar_t* word) {
  // Parametr przekazany, wracamy do UTF-8
  char *uword = g_ucs4_to_utf8((gunichar *)word, -1, NULL, NULL, NULL);
  gboolean result; 

  result = (strcmp(uword, "óroda") != 0);
  g_free(uword);
  return result;
}

void old_dictionary_hints (const struct dictionary *dict, const wchar_t* word,
                       struct word_list *list) {
  char *hints[] = {"broda", "środa", "uroda"};
  int i;

  word_list_init(list);
  for (i = 0; i < 3; i++) {
    wchar_t *item = (wchar_t *)g_utf8_to_ucs4_fast(hints[i], -1, NULL);

    word_list_add(list, item);
    g_free(item);
  }
}

// Procedura ładowania języka

static void ChooseLang (GtkMenuItem *item, gpointer data)
{
    GtkWidget *dialog, *vbox, *label, *combo;
    dialog = gtk_dialog_new_with_buttons("Wybierz...", NULL, 0, 
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_REJECT,
                                         NULL);
    // W treści dialogu dwa elementy
    vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    // Tekst
    label =
      gtk_label_new("Oto lista języków,\ndla których są dostępne słowniki");
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);

    char *list = NULL;
    size_t len = 0;
    if(dictionary_lang_list(&list, &len) < 0) {
      GtkWidget* err_dialog = 
                      gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "Błąd pobierania listy języków!");
      gtk_dialog_run(GTK_DIALOG(err_dialog));
      gtk_widget_destroy(err_dialog);
      return;  
    }

    // Spuszczane menu
    combo = gtk_combo_box_text_new();

    char* lastFirst = list;
    for (size_t i = 0; i < len; i++)
    {
      if ((list + i)[0] == '\0')
      {
        // GtkWidget* err_dialog = 
        //               gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
        //                                      GTK_BUTTONS_OK,
        //                                      lastFirst);
        // gtk_dialog_run(GTK_DIALOG(err_dialog));
        // gtk_widget_destroy(err_dialog);

        //char *uword = g_ucs4_to_utf8((gunichar *)(lastFirst), -1, NULL, NULL, NULL);
      
        // Dodajemy kolejny element
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), lastFirst);
        //g_free(uword);
        
        lastFirst = list + i + 1;
      }
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
    gtk_widget_show(combo);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
       const char* wybranyJezyk
          = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
          fprintf(stderr, ">%s<\n", wybranyJezyk);

        dict_location = wybranyJezyk;

       struct dictionary* tempDict = dictionary_load_lang(wybranyJezyk);
       if(tempDict == NULL)
      {
        GtkWidget* err_dialog = 
                      gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "Nie udało się wczytać słownika!");
        gtk_dialog_run(GTK_DIALOG(err_dialog));
        gtk_widget_destroy(err_dialog);
      }
        if(dict == NULL)
          fprintf(stderr, "dict to null\n");
        if(dict != NULL)
          dictionary_done(dict);

        dict = tempDict;
    }
    gtk_widget_destroy(dialog);

    free(list);
}


// Koloruje słowa nie ze słownika na czerwono

static void ColorMistakes (GtkMenuItem *item, gpointer data) {
  GtkTextIter start, end, prevStart;
  update_actual_dict();

  gtk_text_buffer_get_start_iter(editor_buf, &end);
  gtk_text_buffer_get_end_iter(editor_buf, &start); //inicjalizacja

  gtk_text_buffer_create_tag(editor_buf, "red_fg", 
                             "foreground", "red", 
                             "weight", PANGO_WEIGHT_BOLD, NULL);

  gtk_text_buffer_create_tag(editor_buf, "default_bg", 
                             "foreground", NULL, 
                             NULL, NULL, NULL);

  int i=0;
  //prevStart = end;
  prevStart = start;
  printf("ciagle\n");
  gtk_text_iter_forward_word_end(&end); 
  start = end;
  gtk_text_iter_backward_word_start(&start);

  while (!gtk_text_iter_equal(&prevStart, &start) && (i++ < 50)) {

    gtk_text_buffer_apply_tag_by_name(editor_buf, "default_bg", 
                                      &start, &end);
    char* word = gtk_text_iter_get_text(&start, &end);
    gunichar* wword = g_utf8_to_ucs4_fast(word, -1, NULL);
    g_printf("pocz... word = >%ls<\n", (const wchar_t*)wword);
    if(!dictionary_find(dict, (const wchar_t*)wword))
    {
      printf("przed kolorem\n");
      gtk_text_buffer_apply_tag_by_name(editor_buf, "red_fg", 
                                      &start, &end);
      printf("po kolorze\n");
    }
    printf("...kon\n");
    g_free(word);
    printf("petla %i\n", i);

    prevStart = start;
    printf("ciagle\n");
    gtk_text_iter_forward_word_end(&end); 
    start = end;
    gtk_text_iter_backward_word_start(&start);
  }

}


// Procedurka obsługi

static void WhatCheck (GtkMenuItem *item, gpointer data) {
  GtkWidget *dialog;
  GtkTextIter start, end;
  char *word;
  gunichar *wword;
  
  // Znajdujemy pozycję kursora
  gtk_text_buffer_get_iter_at_mark(editor_buf, &start,
                                   gtk_text_buffer_get_insert(editor_buf));

  // Jeśli nie wewnątrz słowa, kończymy
  if (!gtk_text_iter_inside_word(&start)) {
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_OK,
                                    "Kursor musi być w środku słowa");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }

  // Znajdujemy początek i koniec słowa, a potem samo słowo 
  end = start;
  gtk_text_iter_backward_word_start(&start);
  gtk_text_iter_forward_word_end(&end); 
  word = gtk_text_iter_get_text(&start, &end);

  // Zamieniamy na wide char (no prawie)
  wword = g_utf8_to_ucs4_fast(word, -1, NULL);
  //char* dict_location = "slownik.dict";
  FILE* f = fopen(dict_location, "r");
  if(!f)
  {
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_OK,
              "Nie da się otworzyć pliku słownika!\n Sprawdź, czy plik istnieje"
                                    );
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }

  update_actual_dict();

  // Sprawdzamy
  if (dictionary_find(dict, (wchar_t *)wword)) {
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                    "Wszystko w porządku,\nśpij spokojnie");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
  }
  else {
    // Czas korekty
    GtkWidget *vbox, *label, *combo;
    struct word_list hints;
    int i;
    wchar_t **words;

    dictionary_hints(dict, (wchar_t *)wword, &hints);
    words = word_list_get(&hints);
    dialog = gtk_dialog_new_with_buttons("Korekta", NULL, 0, 
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_REJECT,
                                         NULL);
    // W treści dialogu dwa elementy
    vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    // Tekst
    label = gtk_label_new("Coś nie tak, mam kilka propozycji");
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);

    // Spuszczane menu
    combo = gtk_combo_box_text_new();
    for (i = 0; i < word_list_size(&hints); i++) {
      // Combo box lubi mieć Gtk
      char *uword = g_ucs4_to_utf8((gunichar *)words[i], -1, NULL, NULL, NULL);

      // Dodajemy kolejny element
      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), uword);
      g_free(uword);
    }
    //gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),"<inne...>");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
    gtk_widget_show(combo);

    char *korekta, *question;
    GtkWidget *ask_dialog, *ask_vbox, *ask_label, *err_dialog;
    switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
      case GTK_RESPONSE_ACCEPT:
        korekta =
          gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

        // Usuwamy stare
        gtk_text_buffer_delete(editor_buf, &start, &end);
        // Wstawiamy nowe
        gtk_text_buffer_insert(editor_buf, &start, korekta, -1);
        g_free(korekta);
        break;

      case GTK_RESPONSE_REJECT:
        question = "Czy chcesz dodać to słowo do słownika?";
        ask_dialog = gtk_dialog_new_with_buttons(question, NULL, 0, 
                                                 GTK_STOCK_OK,
                                                 GTK_RESPONSE_ACCEPT,
                                                 GTK_STOCK_CANCEL,
                                                 GTK_RESPONSE_REJECT,
                                                 NULL);
        ask_vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        // Tekst
        ask_label = gtk_label_new("Coś nie tak, mam kilka propozycji");
        gtk_widget_show(ask_label);
        gtk_box_pack_start(GTK_BOX(ask_vbox), ask_label, FALSE, FALSE, 1);

        if (gtk_dialog_run(GTK_DIALOG(ask_dialog)) == GTK_RESPONSE_ACCEPT) {
          dictionary_insert(dict, (wchar_t *)wword);
          fclose(f);
          f = fopen(dict_location, "w");
          if(dictionary_save(dict, f) < 0) {
            err_dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_OK,
                                    "Nie da się zapisać pliku słownika!"
                                              );
              gtk_dialog_run(GTK_DIALOG(err_dialog));
              gtk_widget_destroy(err_dialog);
              //return;
          }
        }
        gtk_widget_destroy(ask_dialog);
        break;

    }
    gtk_widget_destroy(dialog);
  }
  g_free(word);
  g_free(wword);
  fclose(f);
}

// Tutaj dodacie nowe pozycje menu

void extend_menu (GtkWidget *menubar) {
  GtkWidget *spell_menu_item, *spell_menu, *check_item, *lang_load_item,
            *color_mist_item;

  spell_menu_item = gtk_menu_item_new_with_label("Spell");
  spell_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(spell_menu_item), spell_menu);
  gtk_widget_show(spell_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), spell_menu_item);

  check_item = gtk_menu_item_new_with_label("Check Word");
  g_signal_connect(G_OBJECT(check_item), "activate", 
                   G_CALLBACK(WhatCheck), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), check_item);
  gtk_widget_show(check_item);

  lang_load_item = gtk_menu_item_new_with_label("Load lang...");
  g_signal_connect(G_OBJECT(lang_load_item), "activate", 
                   G_CALLBACK(ChooseLang), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), lang_load_item);
  gtk_widget_show(lang_load_item);

  color_mist_item = gtk_menu_item_new_with_label("Color the mistakes");
  g_signal_connect(G_OBJECT(color_mist_item), "activate", 
                   G_CALLBACK(ColorMistakes), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), color_mist_item);
  gtk_widget_show(color_mist_item);
}

/*EOF*/
