#include<iostream>
#include<map>
#include<cmath>
#include<complex>
#include<SFML/Graphics.hpp>

sf::RenderWindow* globWin;

template <typename T>
T mapRangeNumber(T x, T in_min, T in_max, T out_min, T out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int mandelbrot(std::complex<double> x, int max_iter)
{
	int i = 0;
	std::complex<double> a{0.f, 0.f};
	while(std::norm(a) < 4.f && i <= max_iter)
	{
		a = a*a + x;
		i++;
	}
	return i == max_iter ? 0 : i;
}

void updateCnv(
	sf::Image &cnv,
	int    a_max_iter = 100,
	double a_limit = 2.f,
	double a_centerX = 0.f,
	double a_centerY = 0.f
)
{
	int max_iter = a_max_iter;
	double ratio =
	  	static_cast<double>(cnv.getSize().x) /
	  	static_cast<double>(cnv.getSize().y);
	double limit = a_limit;
	double centerX = a_centerX;
	double centerY = a_centerY;
	for(int x=0;x<cnv.getSize().x;x++)
	for(int y=0;y<cnv.getSize().y;y++)
	{
		double mappedX =
		  	mapRangeNumber<double>(
					x,
				  	static_cast<double>(0),
				  	static_cast<double>(cnv.getSize().x),
				  	-limit*ratio + centerX,
				  	limit*ratio + centerX
					);
		double mappedY = 
			mapRangeNumber<double>(
					y,
				  	static_cast<double>(0),
				  	static_cast<double>(cnv.getSize().y),
				  	-limit + centerY,
				  	limit + centerY
					);
		int m = mandelbrot(std::complex<double>( {mappedX, mappedY} ), max_iter);
		cnv.setPixel(x, y, sf::Color(255, 255, 255, 255*m/max_iter));
	}
}

int main(int argc, char* argv[])
{
	const int WIDTH = 1600, HEIGHT = 900;
	sf::RenderWindow win(sf::VideoMode(WIDTH, HEIGHT), "SFML1");
	win.setFramerateLimit(60);
	globWin = &win;

	sf::Clock clock;

	std::map<int, bool> keys;

	sf::Color backgroundColor(sf::Color::Black);

	sf::Image im;
	im.create(win.getSize().x, win.getSize().y);
	updateCnv(im);
	sf::Texture im_t;
	im_t.loadFromImage(im);
	sf::Sprite im_s(im_t);

	double centerX = 0.f;
	double centerY = 0.f;
	double limit = 2.f;
	int max_iter = 30;


	clock.restart();
	while(win.isOpen())
	{
		sf::Time frameStart = clock.restart(); 
		sf::Event evnt;
		while(win.pollEvent(evnt))
		{
			switch(evnt.type)
			{
				case sf::Event::Closed:
					win.close();
				break;
				case sf::Event::KeyPressed:
					keys[evnt.key.code] = true;
				switch(evnt.key.code)
				{
					case sf::Keyboard::Escape:
						win.close();
					break;
					case sf::Keyboard::Q:
						limit *= 1.f-0.1f;
						updateCnv(im, max_iter, limit, centerX, centerY);
					break;
					case sf::Keyboard::A:
						limit *= 1.f+0.1f;
						updateCnv(im, max_iter, limit, centerX, centerY);
					break;
				} break;

				case sf::Event::KeyReleased:
					keys[evnt.key.code] = false;
				break;

				case sf::Event::Resized:
					win.setView(sf::View(sf::FloatRect(0, 0, win.getSize().x, win.getSize().y)));
					im.create(win.getSize().x, win.getSize().y);
					im_t.loadFromImage(im);
					im_s.setTextureRect(sf::IntRect(0, 0, win.getSize().x, win.getSize().y));
					updateCnv(im, max_iter, limit, centerX, centerY);
				break;

				default: break;
			}
		}
		im_t.update(im);


		win.clear(backgroundColor);
		win.draw(im_s);
		win.display();
		sf::Time deltaFrame = clock.getElapsedTime() - frameStart; 
		win.setTitle("FRAME TIME: " +
				std::to_string(1000.f / deltaFrame.asMilliseconds()));
	}
}
