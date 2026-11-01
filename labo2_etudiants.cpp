/* NE PAS TOUCHER */
// ReSharper disable CppUseInternalLinkage
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define THREAD_COUNT 7

/// Variable qui sert à savoir si on a terminé de créer tous les objets
bool done = false;

/// Tableaux tampons pour recevoir les matériaux pour la fabrication et l'affichage.
std::string a[THREAD_COUNT] = {"Free", "Free", "Free", "Free", "Free", "Free", "Free"};
std::string b[THREAD_COUNT] = {"Free", "Free", "Free", "Free", "Free", "Free", "Free"};

/////////////////////////////////////////////////////////////
//AJOUTER VOS INCLUDE ET AUTRES ICI!!!!!!!!!!!!!!!!!!!!!!!!//

#include <algorithm>
#include <cassert>
#include <mutex>

std::mutex craftCheckMutex;

/// Structure holding the information of an ingredient
struct Ingredient
{
	std::string name;
	std::mutex mutex = {};
};

/// Structure holding the information necessary for a craft
struct Recipe
{
	std::string name;
	Ingredient* ingredients[2];
};

Ingredient silver = {.name = "Argent",};
Ingredient gold = {.name = "Or",};
Ingredient wood = {.name = "Bois",};
Ingredient metal = {.name = "Metal",};
Ingredient diamond = {.name = "Diamant",};
Ingredient iron = {.name = "Fer",};
Ingredient jade = {.name = "Jade",};

Recipe recipes[THREAD_COUNT] = {
	{
		.name = "Coupe",
		.ingredients = {&gold, &silver},
	},
	{
		.name = "Porte",
		.ingredients = {&wood, &gold},
	},
	{
		.name = "Table",
		.ingredients = {&metal, &wood},
	},
	{
		.name = "Bague",
		.ingredients = {&diamond, &metal},
	},
	{
		.name = "Chandelier",
		.ingredients = {&iron, &diamond},
	},
	{
		.name = "Épée",
		.ingredients = {&jade, &iron},
	},
	{
		.name = "Bracelet",
		.ingredients = {&silver, &jade},
	},
};

bool compare_recipe(
	const std::pair<int, int>& first,
	const std::pair<int, int>& second
)
{
	return first.second < second.second;
}

/// Gets the recipe at the given index
Recipe* get_recipe(const int index)
{
	assert(index >= 0 && index < THREAD_COUNT);

	return &recipes[index];
}

/////////////////////////////////////////////////////////////


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! NE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

typedef struct
{
	/// Nom des matériaux                              
	std::string name[THREAD_COUNT]{"Argent", "Or", "Bois", "Metal", "Diamant", "Fer", "Jade"};

	/// Nom des matériaux dans une liste               
	std::list<std::string> liste{"Argent", "Or", "Bois", "Metal", "Diamant", "Fer", "Jade"};

	/// Nom des objets fabriqués                
	std::string craft_name[THREAD_COUNT]{"Coupe", "Porte", "Table", "Bague", "Chandelier", "Epee", "Bracelet"};

	///
	int craft[THREAD_COUNT]{0, 0, 0, 0, 0, 0, 0};
} craft_t;

/// Sert pour l'affichage du GUI
Display* dis;

/// Sert pour l'affichage du GUI
int screen;

/// Sert à créer la fenêtre pour le GUI
Window win;

/// graphic context
GC gc;

/// Sert pour les couleurs dans le GUI
unsigned long black, white, red, blue, green;

/// Sert pour les coordonnées                                       
int x, y;

/// Fonction pour initialiser le GUI                                
void init();

/// Fonction pour fermer le GUI et le programme                     
void close();

/// Fonction pour rafraîchir l'affichage graphique                  
void refresh();

/// Fonction pour afficher le menu dans le GUI                      
void menu();

/// Fonction pour afficher les ressources et les objets fabriqués   
void afficher(const craft_t* c);

/// Fonction pour convertir les nombres en couleurs RGB
unsigned long RGB(int r, int g, int b);

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

//

/// Gets the recipe at the given offset
static int get_recipe(const int offset, const craft_t* craft)
{
	constexpr auto size = THREAD_COUNT;

	assert(offset >= 0 && offset < size);
	assert(craft != nullptr);

	std::pair<int, int> recipe_counts[THREAD_COUNT];

	for (int i = 0; i < THREAD_COUNT; ++i)
		recipe_counts[i] = std::pair(i, craft->craft[i]);

	std::ranges::sort(recipe_counts, compare_recipe);

	return recipe_counts[offset].first;
}

void create(const int i, craft_t* c)
{
	// Création itérateur it pour faire une recherche.
	auto it = c->liste.begin();

	// On place l'itérateur à la seconde position de la file pour savoir la valeur.
	advance(it, 1);

	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	//AJOUTER VOTRE CODE ICI.
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	const auto recipe_index = get_recipe(i, c);
	const auto recipe = get_recipe(recipe_index);

	assert(recipe != nullptr);

	craftCheckMutex.lock();
	const auto mutex_index = std::try_lock(
		recipe->ingredients[0]->mutex,
		recipe->ingredients[1]->mutex
	);
	craftCheckMutex.unlock();

	if (mutex_index == -1)
	{
		a[i] = recipe->ingredients[0]->name;
		b[i] = recipe->ingredients[1]->name;

		std::cout << recipe->name << " en création." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		c->craft[recipe_index]++;
		recipe->ingredients[0]->mutex.unlock();
		recipe->ingredients[1]->mutex.unlock();
	}
	else
	{
		std::cout << recipe->name << " en attente de " << recipe->ingredients[mutex_index]->name << " ." << std::endl;
		a[i] = "Waiting";
		b[i] = "Waiting";
	}

	///////////////////////////////////////////////////
}

/*!!!!!!!!!! NE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
void start(craft_t* c)
{
	// On crée un vecteur de threads.
	std::vector<std::thread> threads(THREAD_COUNT);

	// La première boucle crée et utilise un thread et d'appeler la fonction create().
	for (int i = 0; i < THREAD_COUNT; ++i)
		threads[i] = std::thread([i, c] { create(i, c); });

	// La deuxième boucle permet de joindre les threads au fur et à mesure.
	for (auto& t : threads)
		t.join();

	// Ligne permettant de séparer dans le CLI.
	std::cout << "_____________________________________________" << std::endl;
}

/*!!!!!!!!!! NE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int main()
{
	craft_t craft; // Permet de créer l'objet craft, qui va stocker nos informations tout au long du programme.    
	x = 8;
	y = 10; // Permet de configurer les coordonnées de bases pour le GUI.
	init(); // Initilialise le GUI.
	XEvent event; // Permet de savoir le type d'evenement.
	char text[1]; // Permet d'entreposer la valeur du clavier.

	while (true)
	{
		afficher(&craft); // On affiche dans le GUI.
		y = 10; // On remet le y à 10 pour les prochaines affichages.
		XNextEvent(dis, &event); // Se prépare à recevoir l'evenement.

		// Si il y a eu un evenement (en appuyant sur le clavier):
		if (event.type == KeyPress && XLookupString(&event.xkey, text, 255, nullptr, nullptr) == 1)
		{
			// Option "s":
			if (text[0] == 's')
			{
				// Tant que la fabrication n'est pas terminé,
				if (done != 1)
				{
					// On commence un cycle de fabrication d'objet.
					start(&craft);
				}
			}

			// Option "r":
			if (text[0] == 'r')
			{
				// Si la frabrication d'objet n'est pas terminé,
				if (done != 1)
				{
					// On "rafraîchit" l'affichage.
					afficher(&craft);
				}
			}

			// Option "t":
			if (text[0] == 't')
			{
				// Si la frabrication d'objet n'est pas terminé,
				if (done != 1)
				{
					// On "reset" la file en remettant chaque ressources, dans un cas où c'est buggé.
					for (int i = 0; i < THREAD_COUNT; ++i)
					{
						a[i] = b[i] = "Free";
						craft.liste = {"Argent", "Or", "Bois", "Metal", "Diamant", "Fer", "Jade"}; //
					}
				}
			}

			// Option "q":
			if (text[0] == 'q')
			{
				// On ferme le programme.
				close();
			}
		}
	}

	return 0;
}

/*!!!!!!!!!! NE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

void init()
{
	x = 100;
	y = 100;

	dis = XOpenDisplay(nullptr);

	screen = DefaultScreen(dis);

	black = BlackPixel(dis, screen);

	white = WhitePixel(dis, screen);

	red = RGB(255, 0, 0);

	green = RGB(0, 255, 0);

	blue = RGB(0, 0, 255);

	win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, 350, 350, 5, black, white);

	XSetStandardProperties(dis, win, "Lab 2 - 8INF342", nullptr, None, nullptr, 0, nullptr);

	XSelectInput(dis, win, ExposureMask | ButtonPressMask | KeyPressMask);

	gc = XCreateGC(dis, win, 0, nullptr);

	XSetBackground(dis, gc, white);

	XSetForeground(dis, gc, black);

	XClearWindow(dis, win);

	XMapRaised(dis, win);
}


void close()
{
	XFreeGC(dis, gc);

	XDestroyWindow(dis, win);

	XCloseDisplay(dis);

	exit(0);
}


void refresh()
{
	XClearWindow(dis, win);

	x = 8;

	y = 10;
}


void menu()
{
	x = 8;

	y = 10;

	XSetForeground(dis, gc,BlackPixel(dis, screen));

	XDrawString(dis, win, gc, x, y + 200, "Appuyer sur 's' pour cycle", 26);

	XDrawString(dis, win, gc, x, y + 210, "Appuyer 'r' pour refresh page", 25);

	XDrawString(dis, win, gc, x, y + 220, "Appuyer 't' pour reset statut", 29);

	XDrawString(dis, win, gc, x, y + 230, "Appuyer 'q' pour quit", 21);

	XDrawString(dis, win, gc, x, y + 250, "Recipes:", 8);

	XDrawString(dis, win, gc, x, y + 260, "Argent + Or = Coupe", 19);

	XDrawString(dis, win, gc, x, y + 270, "Or + Bois = Porte", 17);

	XDrawString(dis, win, gc, x, y + 280, "Bois + Metal = Table", 20);

	XDrawString(dis, win, gc, x, y + 290, "Metal + Diamant = Bague", 23);

	XDrawString(dis, win, gc, x, y + 300, "Diamant + Fer = Chandelier", 26);

	XDrawString(dis, win, gc, x, y + 310, "Fer + Jade = Epee", 17);

	XDrawString(dis, win, gc, x, y + 320, "Jade + Argent = Bracelet", 24);
}


void afficher(const craft_t* c)
{
	y = 10;

	XClearWindow(dis, win);

	for (int k = 0; k < 7; k++)
	{
		std::string nbr = std::to_string(c->craft[k]);

		XSetForeground(dis, gc,BlackPixel(dis, screen));

		XDrawString(dis, win, gc, x, y, c->craft_name[k].c_str(), c->craft_name[k].size()); // NOLINT(*-narrowing-conversions)

		XDrawString(dis, win, gc, x + 42, y, " : ", 3);

		XSetForeground(dis, gc, blue);

		XDrawString(dis, win, gc, x + 60, y, nbr.c_str(), nbr.size()); // NOLINT(*-narrowing-conversions)

		y = y + 10;
	}

	y = y + 30;

	for (int k = 0; k < 7; k++)
	{
		std::string kk = std::to_string(k + 1);

		XSetForeground(dis, gc,BlackPixel(dis, screen));

		XDrawString(dis, win, gc, x, y, "Objet ", 6);

		XDrawString(dis, win, gc, x + 72, y, kk.c_str(), 1);

		XDrawString(dis, win, gc, x + 76, y, " : ", 3);

		XSetForeground(dis, gc, red);

		XDrawString(dis, win, gc, x + 102, y, a[k].c_str(), a[k].size()); // NOLINT(*-narrowing-conversions)
		a[k] = "Free";

		XDrawString(dis, win, gc, x + 152, y, b[k].c_str(), b[k].size()); // NOLINT(*-narrowing-conversions)
		b[k] = "Free";

		y = y + 10;
	}

	if (done == 1)
	{
		XSetForeground(dis, gc, green);

		XDrawString(dis, win, gc, x, y + 10, "Items completes!", 15);
	}

	menu();
}


unsigned long RGB(const int r, const int g, const int b)
{
	return b + (g << 8) + (r << 16);
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
