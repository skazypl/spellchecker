// Jarosław Socha <js347267>
#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "word_list.h"

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

//dict = NULL;
char dict_location[255] = DEFAULT_LOC;

void update_actual_dict()
{
  GtkWidget *dialog;

  if(dict == NULL)
  {
    if(strcmp(dict_location, DEFAULT_LOC) == 0)
    {
      printf("Nie znaleziono słownika, ładuję pierwszy z folderu\n");
      char* list = NULL;
      size_t len = 0;

      if(dictionary_lang_list(&list, &len) < 0)
      {
        char* err_msg =
        "Błąd ładowania listy słowników!";
        
        dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        err_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return; 
      }
      if(len == 0)
      {
        char* err_msg =
        "Brak słowników!";
        
        dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        err_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return; 
      }
      if((dict = dictionary_load_lang(list)) == NULL)
      {
        char* err_msg =
        "Nie udało się załadować słownika!";
        
        dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        err_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return; 
      }
      strcpy(dict_location, list);
      free(list);
      printf("Uaktualniono słownik - obecnie %s\n", dict_location);
    }
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
        // Dodajemy kolejny element
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), lastFirst);
        
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
       
       //zapisujemy stary słownik - ktoś mógł coś tam dodać i mógł się zmienić
      if(strcmp(dict_location, DEFAULT_LOC) != 0)
        dictionary_save_lang(dict, dict_location);

       // i ładujemy wybrany
      struct dictionary* tempDict = dictionary_load_lang(wybranyJezyk);

      // Jeśli wybrany jest ok, to podmienimy go
      if(tempDict == NULL)
      {
        GtkWidget* err_dialog = 
                      gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "Nie udało się wczytać słownika!");
        gtk_dialog_run(GTK_DIALOG(err_dialog));
        gtk_widget_destroy(err_dialog);
      }
      if(dict != NULL)
        dictionary_done(dict);

      dict = tempDict;
      strcpy(dict_location, wybranyJezyk);
      //free(wybranyJezyk);
    }
    gtk_widget_destroy(dialog);

    free(list);
}


// Koloruje słowa nie ze słownika na czerwono
static void ColorMistakes (GtkMenuItem *item, gpointer data) {
  GtkTextIter start, end, buffEnd;

  gtk_text_buffer_get_start_iter(editor_buf, &end);

  //Teraz sztuczką przesuwamy końcowy iterator na koniec ostatniego słowa
  gtk_text_buffer_get_end_iter(editor_buf, &buffEnd);
  gtk_text_iter_backward_word_start(&buffEnd);
  gtk_text_iter_forward_word_end(&buffEnd);

  gtk_text_buffer_create_tag(editor_buf, "red_fg", 
                             "foreground", "red", 
                             "weight", PANGO_WEIGHT_BOLD, NULL);

  // Aktualizacja słownika
  update_actual_dict();
  start = end;

  while (!gtk_text_iter_equal(&end, &buffEnd)) {
    // Inkrementacja zmiennych
		gtk_text_iter_forward_word_end(&end); 
		start = end;
		gtk_text_iter_backward_word_start(&start);

    // Usuwamy etykietkę ze słowa jeśli jest
    gtk_text_buffer_remove_tag_by_name(editor_buf, "red_fg", 
                                      &start, &end);

    // Separujemy słowo
    char* word = gtk_text_iter_get_text(&start, &end);
    gunichar* wword = g_utf8_to_ucs4_fast(word, -1, NULL);

    // Jeśli znaleziono w słowniku to kolorujemy
    if(!dictionary_find(dict, (const wchar_t*)wword))
      gtk_text_buffer_apply_tag_by_name(editor_buf, "red_fg", 
                                        &start, &end);
    g_free(word);

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

        // Jeśli chiciał dodać nowe słowo do słownika to dodamy i zapiszemy
        if (gtk_dialog_run(GTK_DIALOG(ask_dialog)) == GTK_RESPONSE_ACCEPT) {
          dictionary_insert(dict, (wchar_t *)wword);
          dictionary_save_lang(dict, dict_location);
        }
        
        gtk_widget_destroy(ask_dialog);
        break;
      }
  
    gtk_widget_destroy(dialog);
  }
  
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
