#include<iostream>
#include<map>
#include<cmath>
#include<complex>
#include<SFML/Graphics.hpp>

// Color conversion HSL -> RGB
sf::Color HSLtoRGB(float H, float S, float L)
{
	float C = (1 - std::fabs(2 * L - 1)) * S; // Chroma
	float HPrime = H / 60; // H'
	float X = C * (1 - std::fabs(std::fmod(HPrime, 2.f) - 1));
	float M = L - C / 2;

	float R = 0.f;
	float G = 0.f;
	float B = 0.f;

	switch (static_cast<int>(HPrime))
	{
	case 0: R = C; G = X;        break; // [0, 1)
	case 1: R = X; G = C;        break; // [1, 2)
	case 2:        G = C; B = X; break; // [2, 3)
	case 3:        G = X; B = C; break; // [3, 4)
	case 4: R = X;        B = C; break; // [4, 5)
	case 5: R = C;        B = X; break; // [5, 6)
	}

	R += M;
	G += M;
	B += M;

	sf::Color color;
	color.r = static_cast<sf::Uint8>(std::round(R * 255));
	color.g = static_cast<sf::Uint8>(std::round(G * 255));
	color.b = static_cast<sf::Uint8>(std::round(B * 255));

	return color;
}

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

	// Loop for each pixel on canvas
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
		//m = 255*std::log2(m)/std::log2(max_iter);
		//m = 255*m/max_iter;
		auto pixelColor = HSLtoRGB(360 * std::log2(m)/std::log2(max_iter), 0.7f, 0.6f);  

		cnv.setPixel(x, y, pixelColor);
	}
}

int main(int argc, char* argv[])
{
	// Window initialization
	const int WIDTH = 640, HEIGHT = 360;
	sf::RenderWindow win(sf::VideoMode(WIDTH, HEIGHT), "SFML1");
	win.setFramerateLimit(60.f);
	sf::Color backgroundColor(sf::Color::Black);
	globWin = &win;

	// Clock for timing
	sf::Clock clock;

	// Easy access to real time keyboard input(RTI) using map
	std::map<int, bool> keys;

	// Canvas and texture initialization
	sf::Image im;
	im.create(win.getSize().x, win.getSize().y);
	sf::Texture im_t;
	im_t.loadFromImage(im);
	sf::Sprite im_s(im_t);

	// Parameters used in update function
	double centerX = 0.f;
	double centerY = 0.f;
	double limit = 2.f;
	int max_iter = 30;
	bool isCnvToUpdate = true;

	clock.restart();
	while(win.isOpen())
	{
		sf::Time frameStart = clock.getElapsedTime(); 
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
					keys[event.key.code] = true; // real time input

					// These events are not used for real input 
					switch(event.key.code)
					{
						case sf::Keyboard::Escape:
							win.close();
						break;
					}
				break;

				case sf::Event::KeyReleased:
					keys[event.key.code] = false; // real time event
				break;

				// Updating window rendering if it has been resized
				case sf::Event::Resized:
					win.setView(sf::View(sf::FloatRect(0, 0, win.getSize().x, win.getSize().y)));
					im.create(win.getSize().x, win.getSize().y);
					im_t.loadFromImage(im);
					im_s.setTextureRect(sf::IntRect(0, 0, win.getSize().x, win.getSize().y));
					isCnvToUpdate = true;
				break;

				default: break;
			}
		}
		// Checking what to do with RTI

		// Fractal zooming
		if(keys[sf::Keyboard::E])
		{
			limit *= 1.f-0.1f;
			isCnvToUpdate = true;
		}
		if(keys[sf::Keyboard::Q])
		{	
			limit *= 1.f+0.1f;
			isCnvToUpdate = true;
		}

		// Moving on complex plane
		double deltaMoveFactor = 0.08f*(limit / (limit + 0.2f));
		if(keys[sf::Keyboard::W]) // Up
		{	
			centerY += -deltaMoveFactor;
			isCnvToUpdate = true;
		}
		if(keys[sf::Keyboard::S]) // Down
		{	
			centerY += +deltaMoveFactor;
			isCnvToUpdate = true;
		}
		if(keys[sf::Keyboard::A]) // Left
		{	
			centerX += -deltaMoveFactor;
			isCnvToUpdate = true;
		}
		if(keys[sf::Keyboard::D]) // Right
		{	
			centerX += +deltaMoveFactor;
			isCnvToUpdate = true;
		}

		// Max iteration settings
		if(keys[sf::Keyboard::R])
		{
			max_iter += 1;
			isCnvToUpdate = true;
		}
		if(keys[sf::Keyboard::F])
		{
			if(max_iter != 1)
			{
				max_iter -= 1;
				isCnvToUpdate = true;
			}
		}


		// Texture update from image 
		if(isCnvToUpdate)
		{
			updateCnv(im, max_iter, limit, centerX, centerY);
			im_t.update(im);
			isCnvToUpdate = false;
		}


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
