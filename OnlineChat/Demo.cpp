#include <iostream>



class Animal
{

public:
	virtual void makeSound()
	{
		std::cout << "Sound" << std::endl;
	}

	//virtual void breath() = 0;

};

class Cow : public Animal
{

public:
	void makeSound() override
	{
		std::cout << "SoundCow" << std::endl;
	}

};


int main()
{
	Cow* cow = new Cow();

	cow->makeSound();


}
