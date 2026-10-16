#include "GameMap.h"
#include "character.h"
#include "enemy.h"
#include "event.h"


//locationNode implementations
locationNode::locationNode() {}
locationNode::locationNode(std::string title, std::string desc1, std::string desc2, event* nodeEvent)
{
	//this->name = name;
	this->title = title;
	this->desc1 = desc1;
	this->desc2 = desc2;
	this->eventPtr = nodeEvent;
}

bool locationNode::isLeaf()
{
	if (this->leftPtr == NULL && this->midPtr == NULL && this->rightPtr == NULL) {
		return true;
	}
	return false;
}

bool locationNode::alreadySeen()
{
	if (this->checked == 1) {
		return true;
	}
	return false;
}

void locationNode::steppedFoot() 
{
	this->checked = 1;
}

std::string locationNode::getName()
{
	return name;
}

void locationNode::setName(std::string name)
{
	this->name = name;
}

std::string locationNode::getTitle()
{
	return title;
}

std::string locationNode::getDesc1()
{
	return desc1;
}

std::string locationNode::getDesc2()
{
	return desc2;
}

void locationNode::setLeftChild(locationNode* node)
{
	this->leftPtr = node;
}

void locationNode::setMidChild(locationNode* node)
{
	this->midPtr = node;
}

void locationNode::setRightChild(locationNode* node)
{
	this->rightPtr = node;
}

void locationNode::setParent(locationNode* node)
{
	this->parentPtr = node;
}

void locationNode::setEvent(event* ev)
{
	this->eventPtr = ev;
}

locationNode* locationNode::getRight()
{
	return rightPtr;
}

locationNode* locationNode::getMid()
{
	return midPtr;
}

locationNode* locationNode::getLeft()
{
	return leftPtr;
}

locationNode* locationNode::getParent()
{
	return parentPtr;
}

event* locationNode::getEvent() 
{
	return eventPtr;
}


//gameMap implementations
gameMap::gameMap() {};

int gameMap::getSize()
{
	return size;
}

locationNode* gameMap::getIndex()
{
	return index;
}

locationNode* gameMap::theGamer() 
{
	return user;
}

void gameMap::toRoot()
{
	index = root;
}

void gameMap::current_toRoot(locationNode* current) {
	current = root;
}

void gameMap::addRoot(std::string t, std::string d1, std::string d2, event* ev)
{
	locationNode* newNode = new locationNode(t, d1, d2, ev);
	root = newNode;
	index = root;
	user = root;
	size++;
}

void gameMap::birth_leftChild(locationNode* mommy, std::string t, std::string d1, std::string d2, event* ev)
{
	locationNode* infant = new locationNode(t, d1, d2, ev);
	mommy->setLeftChild(infant);
	infant->setParent(mommy);
	size++;
}

void gameMap::birth_midChild(locationNode* mommy, std::string t, std::string d1, std::string d2, event* ev)
{
	locationNode* infant = new locationNode(t, d1, d2, ev);
	mommy->setMidChild(infant);
	infant->setParent(mommy);
	size++;
}

void gameMap::birth_rightChild(locationNode* mommy, std::string t, std::string d1, std::string d2, event* ev)
{
	locationNode* infant = new locationNode(t, d1, d2, ev);
	mommy->setRightChild(infant);
	infant->setParent(mommy);
	size++;
}

void gameMap::setRusty(locationNode* n) {
	rustyBucket = n;
}
void gameMap::setMailbox(locationNode* n) {
	mailbox = n;
}

void gameMap::moveIndex_left()
{
	index = index->getLeft();
}

void gameMap::moveIndex_middle()
{
	index = index->getMid();
}

void gameMap::moveIndex_right()
{
	index = index->getRight();
}

void gameMap::moveIndex_back()
{
	index = index->getParent();
}

void gameMap::move_left(locationNode* i)
{
	i = i->getLeft();
}
void gameMap::move_middle(locationNode* i)
{
	i = i->getMid();
}
void gameMap::move_right(locationNode* i)
{
	i = i->getRight();
}
void gameMap::move_back(locationNode* i)
{
	i = i->getParent();
}

void gameMap::displayLocation(locationNode* i)
{
	clear();
	color(7);
	size_t nameLen = i->getTitle().length() + 16;
	std::cout << "|--------" << i->getTitle() << "--------|" << std::endl;
	Sdelay(1);
	std::cout << i->getDesc1() << std::endl;
	Sdelay(2);
	std::cout << i->getDesc2() << std::endl;
	Sdelay(1);
	std::cout << "|" << std::string(nameLen, '-') << "|\n";
}

void gameMap::displayLocationAgain(locationNode* i)
{
	clear();
	color(7);
	size_t nameLen = i->getTitle().length() + 16;
	std::cout << "|--------" << i->getTitle() << "--------|" << std::endl;
	std::cout << i->getDesc1() << std::endl;
	std::cout << i->getDesc2() << std::endl;
	std::cout << "|" << std::string(nameLen, '-') << "|\n";
}

// Ultra Deluxe: what you hear when you type a direction (or key) that isn't on the menu.
// The speakers take turns: Janitorius, then Mom, then something Rusty once told you.
static void quickDots()
{
	MSdelay(300); std::cout << "."; MSdelay(300); std::cout << "."; MSdelay(300); std::cout << "."; MSdelay(700);
}

void gameMap::wrongWay(character& c)
{
	static int speaker = 0;
	clear();
	switch (speaker) {
	case 0:	// Janitorius
		color(6);
		std::cout << "Janitorius: You"; quickDots(); std::cout << std::endl;
		std::cout << "can't go that way"; quickDots(); std::cout << std::endl;
		std::cout << "Please try again." << std::endl;
		break;
	case 1:	// Mom
		color(8);
		std::cout << "*You remember your mother's voice*" << std::endl;
		MSdelay(1200);
		color(2);
		std::cout << c.get_name() << " Mom: Honey, I just love you so much." << std::endl;
		MSdelay(1500);
		std::cout << "And we are all great in our own way"; quickDots(); std::cout << std::endl;
		std::cout << "But yours has never been navigation." << std::endl;
		break;
	default:	// Rusty
		color(8);
		std::cout << "*You remember something Rusty told you*" << std::endl;
		MSdelay(1200);
		std::cout << "Rusty: Hey, it's 5 o'clock somewhere"; quickDots(); std::cout << std::endl;
		std::cout << "Hm, I'm not sure if that was helpful"; quickDots(); std::cout << std::endl;
		std::cout << "You should probably try a different input." << std::endl;
		break;
	}
	color(7);
	MSdelay(1800);
	speaker = (speaker + 1) % 3;
}

int gameMap:: Pause_Menu(locationNode* i, character& c)
{
	clear();
	std::string choice;
	while (true)
	{
		std::cout << "|--Pause Menu--|" << std::endl;
		std::cout << "|--------------------------|" << std::endl;
		std::cout << "|"; color(8); std::cout << "[1]: "; color(7); std::cout << "Return Home          |" << std::endl;
		std::cout << "|"; color(8); std::cout << "[2]: "; color(7); std::cout << "Check Inventory	   |" << std::endl;
		std::cout << "|"; color(8); std::cout << "[3]: "; color(7); std::cout << "Exit game            |" << std::endl;
		std::cout << "|"; color(8); std::cout << "[4]: "; color(7); std::cout << "Resume Game          |" << std::endl;
		std::cout << "|--------------------------|" << std::endl;
		std::cin >> choice;

		if (choice == "3") {
			clear();
			std::string exitChoice;
			std::cout << "Are you sure you want to exit? [y/n]" << std::endl;
			std::cin >> exitChoice;
			if (exitChoice == "y") {
				std::string exitChoice1;
				clear();
				Dotdot();
				std::cout << "Are you really really sure? ALL PROGRESS WILL BE LOST! [y/n]" << std::endl;
				std::cin >> exitChoice1;
				if (exitChoice1 == "y") {
					endMusic();
					clear();
					std::cout << "Okay!... Thanks for playing!" << std::endl;
					system("pause");
					c.exitingGame();
					break;
				}
				else
				{
					clear();
					continue;
				}
			}
			else {
				clear();
				continue;
			}
		}
		else if (choice == "1") {
			return 2;
		}
		else if (choice == "2") {
			clear();
			c.checkInventory();
			clear();
			continue;
		}
		else if (choice == "4") {
			return 1;
		}
		else
		{
			clear();
			playMusic("OE.wav");
			std::cout << "WHAT HAVE YOU DONE..." << std::endl;
			Sdelay(5);
			endMusic();
			continue;

		}
	}
	return 0;
}

void gameMap::play(locationNode* i, character& c, int ifPaused) {
	
	if (c.get_lvl() == 10) {
		playMusic("void.wav");
		ellipsis();
		std::cout << " you hear a door opening"; ellipsis(); 
		std::cout << " and someone seems to be screaming in pain... you should go check it out..." << std::endl;
		ellipsis();
		system("pause");
	}

	if (ifPaused == 1) {
		clear();
		displayLocationAgain(i);
	}
	else if (i->getEvent() == NULL) {	//case that will not hold true (all nodes will point to an event)
		playMusic("background.wav");
		displayLocation(i);
	}
	//first display the current location
	else if (i->alreadySeen()) {
		if (i == this->mailbox || i == this->rustyBucket) {
			clear();
			i->getEvent()->trigger(c);
			if (c.get_hp() <= 0) {
				current_toRoot(i);
				c.resetStats();
				play(i, c, 0);
				//if the character is dead
			}
			else if (c.getEnd() == 1) {
				return;
			}
			endMusic();
			playMusic("Mattari.wav");
			displayLocationAgain(i);
		}
		else if (i == this->root) {
			clear();
			i->getEvent()->trigger(c);
			if (c.get_hp() <= 0) {
				current_toRoot(i);
				c.resetStats();
				play(i, c, 0);
				//if the character is dead
			}
			else if (c.getEnd() == 1) {
				return;
			}
			endMusic();
			playMusic("jazzybeat.wav");
			displayLocationAgain(i);
		}
		else {
			clear();
			i->getEvent()->trigger(c);
			if (c.get_hp() <= 0) {
				current_toRoot(i);
				c.resetStats();
				play(i, c, 0);
				//if the character is dead
			}
			else if (c.getEnd() == 1) {
				return;
			}
			endMusic();
			playMusic("background.wav");
			displayLocationAgain(i);
		}
	}
	else {
		if (i == this->mailbox || i == this->rustyBucket) {
			clear();
			i->getEvent()->trigger(c);
			if (c.get_hp() <= 0) {
				current_toRoot(i);
				c.resetStats();
				play(i, c, 0);
			}
			else if (c.getEnd() == 1) {
				return;
			}
			endMusic();
			playMusic("Mattari.wav");
			displayLocation(i);
			i->steppedFoot();
		}
		else if (i == this->root) {
			clear();
			i->getEvent()->trigger(c);
			if (c.get_hp() <= 0) {
				current_toRoot(i);
				c.resetStats();
				play(i, c, 0);
			}
			else if (c.getEnd() == 1) {
				return;
			}
			endMusic();
			playMusic("jazzybeat.wav");
			displayLocation(i);
			i->steppedFoot();
		}
		else {
			clear();
			i->getEvent()->trigger(c);
			if (c.get_hp() <= 0) {
				current_toRoot(i);
				c.resetStats();
				play(i, c, 0);
			}
			else if (c.getEnd() == 1) {
				return;
			}
			endMusic();
			playMusic("background.wav");
			displayLocation(i);
			i->steppedFoot();
		}
	}

	if (c.get_hp() <= 0) {
		return;
	}
	if (c.getLife() == 0) {
		return;//for when you enivitably die
	}
	if (c.getExit() == 1) {
		return;	//if the user chooses to leave the game
	}
	if (c.getEnd() == 1) {
		return;	//if the character finishes the game and defeats final boss
	}
	
	/*displaying the menu*/

	//if current node has all 3 children
	whereText();	//where to next?
	if (i->getLeft() != NULL && i->getMid() != NULL && i->getRight() != NULL) {
		color(8);
		std::cout << "[l]: ";
		color(7);
		std::cout << i->getLeft()->getTitle() << std::endl;
		color(8);
		std::cout << "[m]: ";
		color(7);
		std::cout << i->getMid()->getTitle() << std::endl;
		color(8);
		std::cout << "[r]: ";
		color(7);
		std::cout << i->getRight()->getTitle() << std::endl;
		if (i != root) {
			color(8);
			std::cout << "[b]: ";
			color(7);
			std::cout << "Return to " << i->getParent()->getTitle() << std::endl;
		}
		color(8);
		std::cout << "[p]: ";
		color(7);
		std::cout << "Pause Menu" << std::endl;
		color(8);
		std::cout << "Enter letter:";
		color(7);
	}
	else {	//if there is less than 3 children
		if (i->getLeft() != NULL && i->getMid() != NULL) {
			color(8);
			std::cout << "[l]: ";
			color(7);
			std::cout << i->getLeft()->getTitle() << std::endl;
			color(8);
			std::cout << "[m]: ";
			color(7);
			std::cout << i->getMid()->getTitle() << std::endl;
			if (i != root) {
				color(8);
				std::cout << "[b]: ";
				color(7);
				std::cout << "Return to " << i->getParent()->getTitle() << std::endl;
			}
			color(8);
			std::cout << "[p]: ";
			color(7);
			std::cout << "Pause Menu" << std::endl;
			color(8);
			std::cout << "Enter letter:";
			color(7);

		}
		else if (i->getLeft() != NULL && i->getRight() != NULL) {
			color(8);
			std::cout << "[l]: ";
			color(7);
			std::cout << i->getLeft()->getTitle() << std::endl;
			color(8);
			std::cout << "[r]: ";
			color(7);
			std::cout << i->getRight()->getTitle() << std::endl;
			if (i != root) {
				color(8);
				std::cout << "[b]: ";
				color(7);
				std::cout << "Return to " << i->getParent()->getTitle() << std::endl;
			}
			color(8);
			std::cout << "[p]: ";
			color(7);
			std::cout << "Pause Menu" << std::endl;
			color(8);
			std::cout << "Enter letter:";
			color(7);
		}
		else if (i->getRight() != NULL && i->getMid() != NULL) {
			color(8);
			std::cout << "[r]: ";
			color(7);
			std::cout << i->getRight()->getTitle() << std::endl;
			color(8);
			std::cout << "[m]: ";
			color(7);
			std::cout << i->getMid()->getTitle() << std::endl;
			if (i != root) {
				color(8);
				std::cout << "[b]: ";
				color(7);
				std::cout << "Return to " << i->getParent()->getTitle() << std::endl;
			}
			color(8);
			std::cout << "[p]: ";
			color(7);
			std::cout << "Pause Menu" << std::endl;
			color(8);
			std::cout << "Enter letter:";
			color(7);

		}
		else if (i->getLeft() != NULL) {
			color(8);
			std::cout << "[l]: ";
			color(7);
			std::cout << i->getLeft()->getTitle() << std::endl;
			if (i != root) {
				color(8);
				std::cout << "[b]: ";
				color(7);
				std::cout << "Return to " << i->getParent()->getTitle() << std::endl;
			}
			color(8);
			std::cout << "[p]: ";
			color(7);
			std::cout << "Pause Menu" << std::endl;
			color(8);
			std::cout << "Enter letter:";
			color(7);

		}
		else if (i->getMid() != NULL) {
			color(8);
			std::cout << "[m]: ";
			color(7);
			std::cout << i->getMid()->getTitle() << std::endl;
			if (i != root) {
				color(8);
				std::cout << "[b]: ";
				color(7);
				std::cout << "Return to " << i->getParent()->getTitle() << std::endl;
			}
			color(8);
			std::cout << "[p]: ";
			color(7);
			std::cout << "Pause Menu" << std::endl;
			color(8);
			std::cout << "Enter letter:";
			color(7);

		}
		else if (i->getRight() != NULL) {
			color(8);
			std::cout << "[r]: ";
			color(7);
			std::cout << i->getRight()->getTitle() << std::endl;
			if (i != root) {
				color(8);
				std::cout << "[b]: ";
				color(7);
				std::cout << "Return to " << i->getParent()->getTitle() << std::endl;
			}
			color(8);
			std::cout << "[p]: ";
			color(7);
			std::cout << "Pause Menu" << std::endl;
			color(8);
			std::cout << "Enter letter:";
			color(7);
		}
		else {	//should be leaf case
			if (i != root) {
				color(8);
				std::cout << "[b]: ";
				color(7);
				std::cout << "Return to " << i->getParent()->getTitle() << std::endl;
			}
			color(8);
			std::cout << "[p]: ";
			color(7);
			std::cout << "Pause Menu" << std::endl;
			color(8);
			std::cout << "Enter letter:";
			color(7);
		}
	}
	std::string choose;
	std::cin >> choose;

	// Ultra Deluxe: only move if that room exists (moving into a missing room crashed the game)
	if (choose == "l" && i->getLeft() != NULL) {	//left
		play(i->getLeft(), c, 0);
	}
	else if (choose == "m" && i->getMid() != NULL) {	//middle
		play(i->getMid(), c, 0);
	}
	else if (choose == "r" && i->getRight() != NULL) {	//right
		play(i->getRight(), c, 0);
	}
	else if (choose == "b" && i != root && i->getParent() != NULL) {	//back
		play(i->getParent(), c, 0);
	}
	else if (choose == "p") {	//pause menu
		int pause = Pause_Menu(i, c);
		if (pause == 1) {
			play(i, c, 1);
		}
		else if (pause == 2) {
			i = root;
			play(i, c, 0);
		}
	}
	else {
		// Ultra Deluxe: someone tells you that's not a way you can go, then the room's menu shows again
		// (a wrong key used to replay the room's event, e.g. reopening Putrid's shop or the Rusty Bucket intro)
		wrongWay(c);
		play(i, c, 1);
	}
}
