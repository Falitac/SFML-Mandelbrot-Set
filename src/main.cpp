#include<iostream>
#include<map>
#include<cmath>
#include<complex>
#include<SFML/Graphics.hpp>

// Context for easy access
sf::RenderWindow* globWin;

// Standard mapping function: for given x in range (a, b) convert that number to proper number in range (c, d)
// It works just like in arduino
template <typename T>
T mapRangeNumber(T x, T in_min, T in_max, T out_min, T out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Function which is used to count if given x belongs to mandelbrot set.
// max_iters is like epsilon in numeric operations - it is a factor approximation used in formula.
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

// This function updates canvas by generating mandelbrot set properly to given parameters
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
	// Window initialization
	const int WIDTH = 1600, HEIGHT = 900;
	sf::RenderWindow win(sf::VideoMode(WIDTH, HEIGHT), "SFML1");
	win.setFramerateLimit(60);
	sf::Color backgroundColor(sf::Color::Black);
	globWin = &win;

	// Clock for timing
	sf::Clock clock;

	// Easy access to real time keyboard input using map
	std::map<int, bool> keys;

	// Canvas and texture initialization
	sf::Image im;
	im.create(win.getSize().x, win.getSize().y);
	updateCnv(im);
	sf::Texture im_t;
	im_t.loadFromImage(im);
	sf::Sprite im_s(im_t);

	// Parameters used in update function
	double centerX = 0.f;
	double centerY = 0.f;
	double limit = 2.f;
	int max_iter = 30;

	clock.restart();
	while(win.isOpen())
	{
		sf::Time frameStart = clock.restart(); 
		// Event checking loop
		for(sf::Event event; win.pollEvent(event);)
		{
			switch(event.type)
			{
				// Window exit event
				case sf::Event::Closed:
					win.close();
				break;

				// Keyboard event
				case sf::Event::KeyPressed:
					keys[event.key.code] = true; // real time event

					// These events are not used for real time 
					switch(event.key.code)
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
					}
				break;

				case sf::Event::KeyReleased:
					keys[event.key.code] = false; //real time event
				break;

				// Updating window rendering if it has been resized
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
		// Texture update from image 
		im_t.update(im);


		// Clearing window 
		win.clear(backgroundColor);
		// Drawing things on screen
		win.draw(im_s);

		
		// And finally displaying those things
		win.display();

		//
		sf::Time deltaFrame = clock.getElapsedTime() - frameStart; 
		win.setTitle("FRAME TIME: " +
				std::to_string(1000.f / deltaFrame.asMilliseconds()));
	}
}
