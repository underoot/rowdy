#include <gtk/gtk.h>
#include <libsoup/soup.h>
#include <stdlib.h>

#define USER_AGENT "Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/110.0"

GtkWidget *contentTextView;

static void
activate_entry (GtkEntry *entry, 
                gpointer  user_data)
{
	SoupSession *session = soup_session_new();
	soup_session_set_user_agent(session, USER_AGENT);

	const char *url = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));

	SoupMessage *msg = soup_message_new(SOUP_METHOD_GET, url);
	GError *error = NULL;
	GBytes *bytes = soup_session_send_and_read(
			session,
			msg,
			NULL,
			&error);

	GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);

	if (error)
		gtk_text_buffer_set_text(buffer, 
					 error->message, 
					 strlen(error->message));
	else {
		gsize size = g_bytes_get_size(bytes);
		gtk_text_buffer_set_text(buffer, 
					 (char *)g_bytes_get_data(bytes, &size), 
					 g_bytes_get_size(bytes));
	}

	gtk_text_view_set_buffer(GTK_TEXT_VIEW(contentTextView), buffer);
}

static void
activate (GtkApplication *app,
	  gpointer	  user_data)
{
	GtkWidget *window = gtk_application_window_new (app);
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
	GtkWidget *textEntry = gtk_entry_new ();
	GtkWidget *scrolledWindow = gtk_scrolled_window_new ();

	gtk_window_set_title (GTK_WINDOW(window), "Rowdy");
	gtk_window_set_default_size (GTK_WINDOW(window), 900, 600);

	contentTextView = gtk_text_view_new();
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW(scrolledWindow), 
				       contentTextView);

	gtk_widget_set_margin_top (box, 12);
	gtk_widget_set_margin_end (box, 12);
	gtk_widget_set_margin_bottom (box, 12);
	gtk_widget_set_margin_start (box, 12);

	gtk_box_append (GTK_BOX(box), textEntry);
	gtk_box_append (GTK_BOX(box), scrolledWindow);

	gtk_window_set_child (GTK_WINDOW(window), box);
	gtk_window_present (GTK_WINDOW(window));

	g_signal_connect (textEntry, 
			  "activate", 
			  G_CALLBACK(activate_entry), 
			  NULL);
}

int
main (int    argc,
      char **argv)
{
	GtkApplication *app;
	int status;

	app = gtk_application_new ("dev.underoot.Rowdy", 
				   G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run (G_APPLICATION(app), argc, argv);
	g_object_unref (app);

	return status;
}
