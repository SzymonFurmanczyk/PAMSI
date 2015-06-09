/*!
 *\file 
 *\brief Metody klasy.
 *
 * Plik zawiera metody klasy .
 */

#include "kontener.hh"

#define DVD_SIZE 4707319808

void container::sort()
{
  std::sort(files_test.begin(), files_test.end(),
	    [](const boost::tuple<long, std::string>& a,
	       const boost::tuple<long, std::string>& b) -> bool
	    {
	      return boost::get<0>(a) > boost::get<0>(b);
	    });
  std::cerr << "Wykonano sortowanie." << std::endl;
}


void container::partition()
{
  std::list<disk*>::iterator it;

  if (disks.empty())
    { //jesli nie mamy zadnej plyty
      disk *tmp = new disk();
      disks.push_back(tmp);
    }

  while (true)
    {
      if (files_test.empty()) break;
      bool added = false;
      for (it=disks.begin(); added == false; ++it)
	{
	  if (boost::get<0>(files_test.front()) > DVD_SIZE)
	    { //jesli sie nie zmiesci to do kosza
	      litter.push_back(boost::get<1>(files_test.front())); //nazwa
	      files_test.erase(files_test.begin());
	      added = true;
	    }
	  else if ((*it) -> size + boost::get<0>(files_test.front()) < DVD_SIZE)
	    { //jesli sie zmiesci to zapisujemy
	      (*it) -> size += boost::get<0>(files_test.front()); //rozmiar
	      (*it) -> files.push_back(boost::get<1>(files_test.front())); //nazwa
	      files_test.erase(files_test.begin());
	      added = true;
	    }
	  else if ((*it) == disks.back())
	    { //tworzymy nowa plyte i probujemy jeszcze raz
	      disk *tmp = new disk();
	      disks.push_back(tmp);
	    }
	}
    }
  std::cerr << "Wykonano partycjonowanie." << std::endl;
}

void container::load()
{
  std::fstream file_in;
  file_in.open("zbior_danych.txt", std::ifstream::in);
  while (true)
    {
      if (file_in.eof()) break;
      long tmp1;
      std::string tmp2;
      file_in >> tmp1;
      getline(file_in, tmp2);
      tmp2.erase(0,1);
      boost::tuple<long, std::string> tmp(tmp1, tmp2);
      files_test.push_back(tmp);
    }
  file_in.close();
  std::cerr << "Wczytano rozmiary plikow." << std::endl;
}



void container::save()
{
  std::ofstream file_out;
  file_out.open("rozklad.txt", std::ofstream::out | std::ofstream::trunc);
  while (!disks.empty())
    {
      file_out << "Plyta nr " << disks.size() << std::endl;
      disk *tmp1 = disks.front();
      file_out << "Zajety rozmiar: " << tmp1 -> size << " B" << std::endl;
      while (!(tmp1 -> files.empty()))
	{ //dopoki nie oproznimy plyty
	  std::string tmp2 = (tmp1 -> files.front());
	  tmp1 -> files.pop_front();
	  file_out << (tmp2);
	  file_out << std::endl;
	}
      //po oproznieniu wywalamy plyte
      disks.pop_front();
      file_out << std::endl;
    }
  //jesli jest cos w koszu
  if (!litter.empty()) file_out << "Nierozmieszczone:" << std::endl;
  while (!litter.empty())
    { //dopoki kosz nie jest pusty
      std::string tmp3 = litter.front();
      litter.pop_front();
      file_out << tmp3;
      file_out << std::endl;
    }
  file_out.close();
  std::cerr << "Zapisano konfiguracje ulozenia danych na dyski." << std::endl;
}


void container::show()
{

  sf::ContextSettings settings;
  settings.antialiasingLevel = 2;
  
  sf::RenderWindow window(sf::VideoMode(1024, 800), "Ulozenie plikow:",sf::Style::Close,settings);
  window.setFramerateLimit(60);

  sf::RectangleShape shape(sf::Vector2f(0, 50));
  shape.setFillColor(sf::Color(0, 150, 0));
  shape.setPosition(0,0);
  shape.setOutlineThickness(1);
  shape.setOutlineColor(sf::Color(250, 250, 250));

  sf::Font font;
  font.loadFromFile("DejaVuSans.ttf");
  sf::Text text("", font, 30);

  sf::View view(sf::FloatRect(0, 0, 1024, 800));
  window.setView(view);
  window.display();

  while (window.isOpen())
    {
      sf::Event event;
      while (window.pollEvent(event))
	{
	  switch(event.type)
	    {
	    case  sf::Event::Closed:
	      window.close();
	      break;

	    case  sf::Event::MouseButtonPressed:
	      if (event.mouseButton.button == sf::Mouse::Left)
		{
		  sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
		  sf::Vector2f coord_pos = window.mapPixelToCoords(pixel_pos);
		  std::cout << "mouse x: " << coord_pos.x << std::endl;
		  std::cout << "mouse y: " << coord_pos.y << std::endl;
		}
	      else if (event.mouseButton.button == sf::Mouse::Right)
		{
		  sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
		  sf::Vector2f coord_pos = window.mapPixelToCoords(pixel_pos);
		  std::cout << "mouse x: " << coord_pos.x << std::endl;
		  std::cout << "mouse y: " << coord_pos.y << std::endl;
		}
	      break;

	    case sf::Event::MouseWheelMoved:

	      if(view.getCenter().y+event.mouseWheel.delta*-20 < 400)
		{
		  
		}
	      else
		{
		  view.move(0,event.mouseWheel.delta*-20);
	      
		  if(view.getCenter().y>=400)
		    {
		      window.setView(view);
		      window.clear();
		      std::list<disk*>::iterator it;
		      int i=0;
		      int koniec=0;
		      for (it=disks.begin();koniec!=1; it++)
			{
			  if(it==disks.end())
			    {
			      text.setString("Nieumieszczone:");
			      text.setPosition(sf::Vector2f(0,i*55));
			      window.draw(text);
			      koniec=1;
			    }
			  else
			    {
			      shape.setPosition(0,i*55);
			      shape.setSize(sf::Vector2f(((*it)->size*1024)/DVD_SIZE, 50));

			      if((*it)->size>=DVD_SIZE-12)
				{
				  shape.setFillColor(sf::Color(0, 150, 0));
				}
			      else
				{
				  shape.setFillColor(sf::Color(150, 0, 0));
				}

			      window.draw(shape);
			  
			      std::stringstream ss;
			      ss<<"Dysk: "<<i<<" pozostalo miejsca: " <<DVD_SIZE-(*it)->size;
			      std::string str = ss.str();

			      text.setString(str);
			      text.setPosition(sf::Vector2f(0,i*55));
			      window.draw(text);

			    }
			  i++;
			}
		    }
		}
	      break;
	      
	    default:
	      break;
	    }
        }
      window.display();
    }
    
}
