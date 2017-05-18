#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>

GtkListStore *pointertostore; // pointer to store
char **argvp;  // pointer to argv

#define TEST   printf( "HERE at line[%d]\n", __LINE__); // macro used for debugging only
#define MAX_LINE 1024

int proc();

enum {
  PROCESS_NAME = 0,
  USER,
  CPU,
  ID,
  MEMORY,
  IMAGE,
  COLUMNS
};

/////////////////////////////////////////////////////////////////////////

int build_list (GtkListStore *store) {

    char filename[] = "data.txt";
    FILE *input;

    // check the mode to see if you can read the file
    if ((input = fopen(filename, "r")) == NULL) {
        printf("File %s not found\n", filename);
        return 1;
    }

    char line[MAX_LINE];
    char data[COLUMNS][MAX_LINE];
    char *token;
    int col;
    gint id;
    gfloat memory;
    glong cpu;
    gboolean isroot;
    GError *error = NULL;
    GdkPixbuf* image1 = gdk_pixbuf_new_from_file("root.png", &error);
    GdkPixbuf* image2 = gdk_pixbuf_new_from_file("user.png", &error);
    GdkPixbuf* image;

    GtkTreeIter iter;

    // for each line of input
    while (fgets(line, MAX_LINE, input)) {
        line[strlen(line)-1] = '\0';
        token = strtok(line, " ");
        col = 0;


        // for each string in the line put the token in data[]
        while (token != NULL) {
            strcpy(data[col], token);
            token = strtok(NULL, " ");
            col++;
        }

        // add the data as a new row in the list store
        gtk_list_store_append (store, &iter);
        sscanf ( data[CPU], "%lu" , &cpu);
        sscanf (data[MEMORY], "%f", &memory); // scan for a float and store it in height address
        strcat(data[MEMORY], " kB");
        //printf("%s\n", data[MEMORY]);
        sscanf(data[ID], "%d", &id);
        isroot = (strcmp(data[USER], "root"));
        if (0 == isroot) {
          image = image1;
        }
        else {
          image = image2;
        }

        gtk_list_store_set (store, &iter, PROCESS_NAME, data[PROCESS_NAME],
                            USER, data[USER],
                            CPU, cpu,
                            ID, id,
                            MEMORY, memory,
                            IMAGE, image,
                             -1);
    }
    return 0;
} // EObuild_list

/////////////////////////////////////////////////////////////////////////

void memory_cell_display (GtkTreeViewColumn* col,
                       GtkCellRenderer* renderer,
                       GtkTreeModel* model,
                       GtkTreeIter* iter,
                       gpointer data )  {
    gfloat memory;
    gchar buf[20]; // to store the height in a buffer

    gtk_tree_model_get (model, iter, MEMORY, &memory, -1);
    g_snprintf (buf, 20, "         %.1f", memory);
    g_object_set (renderer, "text", buf, NULL); // render what is stored in the buffer

}

/////////////////////////////////////////////////////////////////////////

void build_treeview (GtkWidget *treeview) {

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // add the Process Name to the treeview, with an icon
    column = gtk_tree_view_column_new();
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", IMAGE, NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes (column, renderer, "text", PROCESS_NAME,
                NULL);

    gtk_tree_view_column_set_title(column, "Process Name");
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    //****************************************************added for sorting
    // set the sort column and indicator
    gtk_tree_view_column_set_sort_column_id(column, PROCESS_NAME);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);
    //***************************************************

    // add the User to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "User", renderer, "text", USER,
                NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
    //****************************************************added for sorting
    // set the sort column and indicator
    gtk_tree_view_column_set_sort_column_id(column, USER);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);
    //***************************************************

    // add the %CPU to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "%CPU", renderer, "text", CPU,
                NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    //****************************************************added for sorting
    // set the sort column and indicator
    gtk_tree_view_column_set_sort_column_id(column, CPU);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);
    //***************************************************

    // add the ID to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "ID", renderer, "text", ID,
                NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    //****************************************************added for sorting
    // set the sort column and indicator
    gtk_tree_view_column_set_sort_column_id(column, ID);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);
    //***************************************************

    // add the memory to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new ();
    gtk_tree_view_column_set_title (column, "Memory kB");
    gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), column);
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func (column, renderer, memory_cell_display,
                                             NULL, NULL);
    //****************************************************added for sorting
    // set the sort column and indicator
    gtk_tree_view_column_set_sort_column_id(column, MEMORY);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);
    //***************************************************
}

/////////////////////////////////////////////////////////////////////////

void display (GtkWidget *treeview) {

    // create the window
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Process Manager");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 700, 700);
    g_signal_connect (window, "delete_event", gtk_main_quit, NULL);

    // create a scrolled window
    GtkWidget* scroller = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);

    // pack the containers
    gtk_container_add (GTK_CONTAINER (scroller), treeview);
    gtk_container_add (GTK_CONTAINER (window), scroller);
    gtk_widget_show_all (window);

}

/////////////////////////////////////////////////////////////////////////

// define sort order on Process Name
gint sort_process_names (GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {

    gchar *name1, *name2;
    gtk_tree_model_get (model, a, PROCESS_NAME, &name1, -1);
    gtk_tree_model_get (model, b, PROCESS_NAME, &name2, -1);

    int order = strcmp(name1, name2);
    g_free(name1);
    g_free(name2);

    return -order;
}

/////////////////////////////////////////////////////////////////////////

// define sort order on User
gint sort_user_names (GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {

    gchar *name1, *name2;
    gtk_tree_model_get (model, a, USER, &name1, -1);
    gtk_tree_model_get (model, b, USER, &name2, -1);

    int order = strcmp(name1, name2);
    g_free(name1);
    g_free(name2);

    return -order;
}


/////////////////////////////////////////////////////////////////////////

// define sort order on ID numbers
gint sort_id(GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {

    gint num1, num2;
    gtk_tree_model_get (model, a, ID, &num1, -1);
    gtk_tree_model_get (model, b, ID, &num2, -1);

    int order;

    if (num1 > num2){
       order = 1;
    }
    else if (num1 < num2 ) {
       order = -1;
    }
    else {
       order = 0;
    }

    return -order;
}

/////////////////////////////////////////////////////////////////////////

// define sort order on memory
gint sort_memory (GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {

    gfloat num1, num2;
    gtk_tree_model_get (model, a, MEMORY, &num1, -1);
    gtk_tree_model_get (model, b, MEMORY, &num2, -1);

    int order;

    if (num1 > num2){
       order = 1;
    }
    else if (num1 < num2 ) {
       order = -1;
    }
    else {
       order = 0;
    }

    return -order;
}

/////////////////////////////////////////////////////////////////////////

// define sort order on cpu
gint sort_cpu (GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {

    gfloat num1, num2;
    gtk_tree_model_get (model, a, CPU, &num1, -1);
    gtk_tree_model_get (model, b, CPU, &num2, -1);

    int order;

    if (num1 > num2){
       order = 1;
    }
    else if (num1 < num2 ) {
       order = -1;
    }
    else {
       order = 0;
    }

    return -order;
}

/////////////////////////////////////////////////////////////////////////

// time-out call-back function
static gboolean update_timer (GtkWidget *treeview) {

  gtk_list_store_clear(pointertostore);
  proc();
  //build_list(argvp[1], pointertostore);
  build_list(pointertostore);

	return TRUE;

}

/////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

  // set a global pointer to argv file name
  argvp = argv;

  gtk_init (&argc, &argv);

  // build the list store from the file data
  GtkListStore *store = gtk_list_store_new (COLUMNS, G_TYPE_STRING,
                                            G_TYPE_STRING, G_TYPE_LONG, G_TYPE_INT,
                                            G_TYPE_FLOAT, GDK_TYPE_PIXBUF);
  // set a global pointer to store
  pointertostore = store;

 //********************for sorting*****************************************************************************
GtkTreeSortable *sortable = GTK_TREE_SORTABLE (store);

// sort for Process Name
gtk_tree_sortable_set_sort_func (sortable, PROCESS_NAME, sort_process_names, GINT_TO_POINTER (PROCESS_NAME), NULL);
gtk_tree_sortable_set_sort_column_id (sortable, PROCESS_NAME, GTK_SORT_ASCENDING);

// sort for User
gtk_tree_sortable_set_sort_func (sortable, USER, sort_user_names, GINT_TO_POINTER (USER), NULL);
gtk_tree_sortable_set_sort_column_id (sortable, USER, GTK_SORT_ASCENDING);

// sort for ID
gtk_tree_sortable_set_sort_func (sortable, ID, sort_id, GINT_TO_POINTER (ID), NULL);
gtk_tree_sortable_set_sort_column_id (sortable, ID, GTK_SORT_ASCENDING);

//sort for cpu
gtk_tree_sortable_set_sort_func (sortable, CPU, sort_cpu, GINT_TO_POINTER (CPU), NULL);
gtk_tree_sortable_set_sort_column_id (sortable, CPU, GTK_SORT_ASCENDING);

// sort for Memory
gtk_tree_sortable_set_sort_func (sortable, MEMORY, sort_memory, GINT_TO_POINTER (MEMORY), NULL);
gtk_tree_sortable_set_sort_column_id (sortable, MEMORY, GTK_SORT_ASCENDING);
//********************end sorting******************************************************************************

  // create the tree view of the list
  GtkWidget *treeview = gtk_tree_view_new ();
  display (treeview);
  build_treeview(treeview);

  // add the tree model to the tree view
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));

  /* Create a timer */
  g_timeout_add (1000, (GSourceFunc) update_timer, (gpointer) treeview);

  // unreference the model so that it will be destroyed when the tree
  // view is destroyed
  g_object_unref (store);

  gtk_main ();

return 0;
} // EOmain
