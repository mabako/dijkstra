#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <queue>
#include <list>

#define MAX_ENTFERNUNG 10000 // (size_t)-1;

using namespace std;

class vertex
{
private:
	// name des aktuellen Knotens
	string name;

	// Alle Nachbarknoten und deren Entfernungen
	map<vertex*, size_t> nachbarn;

	// Vorgänger & Distanz dazu
	vertex* vorgaenger;
	size_t entfernung_zum_vorgaenger;

public:
	// Speichert den Namen
	vertex(string& name) : name(name)
	{
	}

	// Setzt alle Werte für die Streckenberechnung auf Anfangswerte.
	void reset()
	{
		vorgaenger = nullptr;
		entfernung_zum_vorgaenger = MAX_ENTFERNUNG;
	}

	// Aktualisiert den Vorgänger/Entfernung
	void update(vertex* vorgaenger_, size_t entfernung)
	{
		// Kürzere Strecke?
		if (entfernung < entfernung_zum_vorgaenger)
		{
			vorgaenger = vorgaenger_;
			entfernung_zum_vorgaenger = entfernung;

			cout << "    Setze Entfernung von " << (vorgaenger ? vorgaenger->getName() : "<START>") << " -> " << name << " auf " << entfernung << endl;
		}
	}

	string getName() const
	{
		return name;
	}

	size_t getEntfernung() const
	{
		return entfernung_zum_vorgaenger;
	}

	vertex* getVorgaenger() const
	{
		return vorgaenger;
	}

	// Gibt alle Nachbarn zurück
	const map<vertex*, size_t>& getNachbarn() const
	{
		return nachbarn;
	}

	// Fügt einen neuen Nachbarort ein.
	void neuerNachbar(vertex* nachbar, size_t entfernung)
	{
		if (nachbarn.count(nachbar) == 0)
		{
			nachbarn[nachbar] = entfernung;
		}
		else
		{
			cout << "Nachbarort " << nachbar->name << " von " << name << " ist bereits bekannt." << endl;
		}
	}
};

bool Vergleich(const vertex* lhs, const vertex* rhs)
{
	return lhs->getEntfernung() > rhs->getEntfernung();
}

void dijkstra(map<string, vertex*>& karte, vertex* start, vertex* ziel)
{
	// Alle besuchten Knoten speichern
	list<vertex*> besucht;

	start->update(nullptr, 0);

	// Heap statt priority_queue, da sich die Entfernungen im Laufe der Zeit ändern. Alle Elemente einfügen.
	vector<vertex*> heap;
	for (auto k : karte) heap.push_back(k.second);

	while (!heap.empty())
	{
		// Heap neu sortieren (am Anfang nötig, da Elemente unsortiert; bei jeder weitern Iteration wegen neuen Entfernungen nötig)
		make_heap(heap.begin(), heap.end(), Vergleich);

		cout << "    Aktuelle Entfernungen: ";
		for (unsigned i = 0; i<heap.size(); i++)
			cout << heap[i]->getEntfernung() << ' ';
		cout << endl;

		vertex* aktuell = heap.front();
		pop_heap(heap.begin(), heap.end(), Vergleich);
		heap.pop_back();

		// wurde jetzt besucht
		cout << "  Besuche Ort " << aktuell->getName() << " (Entfernung " << aktuell->getEntfernung() << ")" << endl;
		besucht.push_back(aktuell);

		// Entfernung des aktuellen Knotens
		size_t entfernung = aktuell->getEntfernung();

		if (entfernung >= MAX_ENTFERNUNG)
		{
			cout << "Maximale Entfernung bei Suche nach " << ziel->getName() << " erreicht." << endl;
			return;
		}
		if (aktuell == ziel)
		{
			cout << "Kuerzeste Route von " << start->getName() << " nach " << ziel->getName() << " ist " << entfernung << endl;
			
			// Mit Vorgänger kommen wir nur auf die umgekehrte Reihenfolge... also umdrehen
			list<vertex*> strecke;
			do {
				strecke.push_front(aktuell);
				aktuell = aktuell->getVorgaenger();
			} while (aktuell != nullptr);

			for (vertex* v : strecke)
				cout << "  " << v->getName() << endl;
			cout << endl;

			return;
		}
		else
		{
			// alle Nachbarn einfügen, sofern diese noch nicht besucht wurden.
			for (auto n : aktuell->getNachbarn())
			{
				vertex* nachbar = n.first;
				// Wurde dieser Nachbar noch nicht besucht?
				if (find(besucht.cbegin(), besucht.cend(), nachbar) == besucht.cend())
				{
					size_t entfernung_zum_nachbar = n.second + entfernung;
					nachbar->update(aktuell, entfernung_zum_nachbar);
				}
			}

		}
	}
}

int main()
{
	cout << "Alle Orte einlesen..." << endl;

	ifstream datei("../dijkstra/uk.txt");
	if (!datei.good())
	{
		cout << "Datei konnte nicht geöffnet werden." << endl;
		return 0;
	}

	// Orte nach Namen sortiert.
	map<string, vertex*> karte;

	// Anzahl eingelesener Routen
	size_t routen = 0;

	// Datei einlesen
	string zeile;
	while (getline(datei, zeile))
	{
		// Sollte schon was auf der Zeile sein
		if (zeile.length() < 3)
			continue;

		stringstream ss(zeile);
		string token;
		ss >> token;

		if (token == "V")
		{
			// Vertex, d.h. ein einzelner Ort.
			string ort;
			ss >> ort;

			karte[ort] = new vertex(ort);

			cout << "* " << karte[ort]->getName() << endl;
		}
		else if (token == "E")
		{
			// Strecke zwischen zwei Orten
			string ort1, ort2;
			size_t entfernung;

			ss >> ort1 >> ort2 >> entfernung;

			vertex* v1 = karte[ort1];
			vertex* v2 = karte[ort2];
			if (!v1)
			{
				cout << "Unbekannter Ort (1) " << ort1;
				return 0;
			}
			else if (!v2)
			{
				cout << "Unbekannter Ort (2) " << ort2;
				return 0;
			}
			else
			{
				v1->neuerNachbar(v2, entfernung);
				v2->neuerNachbar(v1, entfernung);

				cout << (++ routen) << ".) " << ort1 << " <- " << entfernung << " -> " << ort2 << endl;
			}
		}
		else if (token == "R")
		{
			// Routenberechnung
			string ort1, ort2;
			ss >> ort1 >> ort2;

			// Sind beide Orte überhaupt auf der Karte?
			vertex* start = karte[ort1];
			vertex* ziel = karte[ort2];

			if (!start)
			{
				cout << "Unbekannter Ort (1) " << ort1;
				return 0;
			}
			else if (!ziel)
			{
				cout << "Unbekannter Ort (2) " << ort2;
				return 0;
			}
			else
			{
				cout << endl << endl << "Routenberechnung von " << ort1 << " nach " << ort2 << endl;

				// Karte zurücksetzen (so sind mehrere Routenberechnungen möglich)
				for (auto v : karte) v.second->reset();

				dijkstra(karte, start, ziel);
			}
		}
	}
}
