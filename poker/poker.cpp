// Poker Game
// Copyright 2015 Daniel Harding
// Licensed under the DBAD Public License
// http://www.dbad-license.org/

#include <stdio.h>
#include <conio.h>
#include <ctime>
#include <Windows.h>

const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

const int NUM_VALUES  = 13;
const int NUM_SUITS   = 4;
const int NUM_CARDS   = 5;
const int NUM_DISCARD = 3;

const COORD DEALER_C = { 8,  1 };
const COORD PLAYER_C = { 8,  7 };
const COORD DONE_C   = { 9, 11 };
const COORD WINNER_C = { 6, 13 };
const COORD AGAIN_C  = { 0, 16 };

enum VALUE {
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN,
	JACK,
	QUEEN,
	KING,
	ACE
};

enum SUIT {
	CLUBS,
	DIAMONDS,
	HEARTS,
	SPADES
};

enum RANK {
	HIGH,
	PAIR,
	TWO_PAIR,
	THREE_KIND,
	STRAIGHT,
	FLUSH,
	FULL_HOUSE,
	FOUR_KIND,
	STRAIGHT_FLUSH,
	ROYAL_FLUSH
};

enum WINNER {
	PLAYER,
	DEALER,
	TIE
};

VALUE& operator++(VALUE &v)
{
	if (v < ACE)
	{
		v = (VALUE)((int)v + 1);
	}
	else
	{
		v = TWO;
	}
	return v;
}

VALUE& operator--(VALUE &v)
{
	if (v > TWO)
	{
		v = (VALUE)((int)v - 1);
	}
	else
	{
		v = ACE;
	}
	return v;
}

SUIT& operator++(SUIT &s)
{
	if (s < SPADES)
	{
		s = (SUIT)((int)s + 1);
	}
	else
	{
		s = CLUBS;
	}
	return s;
}

SUIT& operator--(SUIT &s)
{
	if (s > CLUBS)
	{
		s = (SUIT)((int)s - 1);
	}
	else
	{
		s = SPADES;
	}
	return s;
}

class DECK;

class HAND {
	friend class DECK;
	friend void PickHand(HAND&, int);
	friend void DisplayHand(HAND&, int);
	friend void DisplayRank(HAND&, int);
	friend WINNER DetermineWinner(HAND&, HAND&);

public:
	HAND();
	void Sort();
	void DetermineRank();

private:
	VALUE values[NUM_CARDS];
	VALUE values_sorted[NUM_CARDS];
	SUIT suits[NUM_CARDS];
	RANK rank;
	VALUE primary;
	VALUE secondary;

	bool IsRoyalFlush();
	bool IsStraightFlush();
	bool IsFourKind();
	bool IsFullHouse();
	bool IsFlush();
	bool IsStraight();
	bool IsThreeKind();
	bool IsTwoPair();
	bool IsPair();
};

class DECK {
public:
	DECK();
	void DealHand(HAND&);
	void DrawCard(HAND&, int);

private:
	bool available[NUM_VALUES][NUM_SUITS];
};

void HideCursor();
void PlayGame();
bool PlayAgain();
bool Debug();
void PickHand(HAND&, int);
void DiscardPlayerCards(DECK&, HAND&);
void DiscardDealerCards(DECK&, HAND&);
void DisplayHands(HAND&, HAND&, bool);
void DisplayHand(HAND&, int);
void DisplayWinner(WINNER, HAND&, HAND&);
void DisplayRank(HAND&, int);
WINNER DetermineWinner(HAND&, HAND&);

int main()
{
	srand((int)time(NULL));
	HideCursor();
	do
	{
		PlayGame();
	} while (PlayAgain());
	return 0;
}

void HideCursor()
{
	CONSOLE_CURSOR_INFO cursor;
	GetConsoleCursorInfo(handle, &cursor);
	cursor.bVisible = false;
	SetConsoleCursorInfo(handle, &cursor);
}

void PlayGame()
{
	DECK deck;
	HAND player;
	HAND dealer;
	WINNER winner;
	system("CLS");
	if (Debug())
	{
		DisplayHands(player, dealer, true);
		PickHand(dealer, DEALER_C.Y + 1);
		PickHand(player, PLAYER_C.Y + 1);
	}
	else
	{
		deck.DealHand(player);
		deck.DealHand(dealer);
		DisplayHands(player, dealer, false);
		DiscardPlayerCards(deck, player);
		DiscardDealerCards(deck, dealer);
	}
	player.Sort();
	dealer.Sort();
	player.DetermineRank();
	dealer.DetermineRank();
	winner = DetermineWinner(player, dealer);
	DisplayHands(player, dealer, true);
	DisplayWinner(winner, player, dealer);
}

bool PlayAgain()
{
	COORD cursor;
	cursor.Y = AGAIN_C.Y;
	cursor.X = AGAIN_C.X;
	SetConsoleCursorPosition(handle, cursor);
	printf("WOULD YOU LIKE TO PLAY AGAIN?");

	cursor.Y = AGAIN_C.Y + 1;
	SetConsoleCursorPosition(handle, cursor);
	printf("  YES  NO");

	cursor.X = AGAIN_C.X + 1;
	SetConsoleCursorPosition(handle, cursor);
	printf("%c", 0xAF);

	char key;
	int x = 0;
	const int X_MAX = 1;
	bool done = false;
	do
	{
		key = tolower(_getch());
		switch (key)
		{
		case ('a'):
		case ('d'):
		{
			x += key == 'a' ? -1 : 1;
			if (x < 0)
			{
				x = X_MAX;
			}
			else if (x > X_MAX)
			{
				x = 0;
			}
			SetConsoleCursorPosition(handle, cursor);
			printf(" ");
			cursor.X = 1 + x * 5;
			SetConsoleCursorPosition(handle, cursor);
			printf("%c", 0xAF);
			break;
		}
		case ('\r'):
		{
			done = true;
			break;
		}
		default:
		{
			break;
		}
		}
	} while (!done);
	if (x == 0)
	{
		return true;
	}
	return false;
}

bool Debug()
{
	COORD cursor;
	cursor.Y = 0;
	cursor.X = 0;
	SetConsoleCursorPosition(handle, cursor);
	printf("DEBUG?");

	cursor.Y = 1;
	SetConsoleCursorPosition(handle, cursor);
	printf("  YES  NO");

	cursor.X = 1;
	SetConsoleCursorPosition(handle, cursor);
	printf("%c", 0xAF);

	char key;
	int x = 0;
	const int X_MAX = 1;
	bool done = false;
	do
	{
		key = tolower(_getch());
		switch (key)
		{
		case ('a'):
		case ('d'):
		{
			x += key == 'a' ? -1 : 1;
			if (x < 0)
			{
				x = X_MAX;
			}
			else if (x > X_MAX)
			{
				x = 0;
			}
			SetConsoleCursorPosition(handle, cursor);
			printf(" ");
			cursor.X = 1 + x * 5;
			SetConsoleCursorPosition(handle, cursor);
			printf("%c", 0xAF);
			break;
		}
		case ('\r'):
		{
			done = true;
			break;
		}
		default:
		{
			break;
		}
		}
	} while (!done);
	cursor.Y = 0;
	cursor.X = 0;
	SetConsoleCursorPosition(handle, cursor);
	printf("      ");
	cursor.Y = 1;
	SetConsoleCursorPosition(handle, cursor);
	printf("         ");
	if (x == 0)
	{
		return true;
	}
	return false;
}

void PickHand(HAND &hand, int baseY)
{
	COORD cursor;
	cursor.Y = baseY;
	cursor.X = 2;
	SetConsoleCursorPosition(handle, cursor);
	printf("^");
	cursor.Y += 2;
	SetConsoleCursorPosition(handle, cursor);
	printf("v");
	cursor.Y -= 2;

	char key;
	int x = 0;
	const int X_MAX = NUM_CARDS * 2 - 1;
	bool done = false;
	do
	{
		key = tolower(_getch());
		switch (key)
		{
		case ('a'):
		case ('d'):
		{
			x += key == 'a' ? -1 : 1;
			if (x < 0)
			{
				x = X_MAX;
			}
			else if (x > X_MAX)
			{
				x = 0;
			}
			SetConsoleCursorPosition(handle, cursor);
			printf(" ");
			cursor.Y += 2;
			SetConsoleCursorPosition(handle, cursor);
			printf(" ");
			cursor.Y -= 2;

			cursor.X = 2 + x * 2 - x % 2;
			SetConsoleCursorPosition(handle, cursor);
			printf("^");
			cursor.Y += 2;
			SetConsoleCursorPosition(handle, cursor);
			printf("v");
			cursor.Y -= 2;
			break;
		}
		case ('w'):
		{
			int card = x / 2;
			if (x % 2)
			{
				++hand.suits[card];
			}
			else
			{
				++hand.values[card];
			}
			DisplayHand(hand, baseY + 1);
			break;
		}
		case ('s'):
		{
			int card = x / 2;
			if (x % 2)
			{
				--hand.suits[card];
			}
			else
			{
				--hand.values[card];
			}
			DisplayHand(hand, baseY + 1);
			break;
		}
		case ('\r'):
		{
			done = true;
			SetConsoleCursorPosition(handle, cursor);
			printf(" ");
			cursor.Y += 2;
			SetConsoleCursorPosition(handle, cursor);
			printf(" ");
			cursor.Y -= 2;
			break;
		}
		default:
		{
			break;
		}
		}
	} while (!done);
}

void DiscardPlayerCards(DECK &deck, HAND &hand)
{
	if (NUM_DISCARD)
	{
		COORD cursor;

		SetConsoleCursorPosition(handle, DONE_C);
		printf("DONE");

		cursor.Y = PLAYER_C.Y + 2;
		cursor.X = 1;
		SetConsoleCursorPosition(handle, cursor);
		printf("%c", 0xAF);

		char key;
		int x = 0, y = 0;
		const int X_MAX = NUM_CARDS - 1;
		const int Y_MAX = 1;
		int numDiscards = 0;
		bool discards[NUM_CARDS];
		bool done = false;
		for (int card = 0; card < NUM_CARDS; ++card)
		{
			discards[card] = false;
		}
		do
		{
			key = tolower(_getch());
			switch (key)
			{
			case ('a'):
			case ('d'):
			{
				if (y == 0)
				{
					x += key == 'a' ? -1 : 1;
					if (x < 0)
					{
						x = X_MAX;
					}
					else if (x > X_MAX)
					{
						x = 0;
					}
					SetConsoleCursorPosition(handle, cursor);
					printf(" ");
					cursor.X = 1 + x * 4;
					SetConsoleCursorPosition(handle, cursor);
					printf("%c", 0xAF);
				}
				break;
			}
			case ('w'):
			case ('s'):
			{
				y += key == 'w' ? -1 : 1;
				if (y < 0)
				{
					y = Y_MAX;
				}
				else if (y > Y_MAX)
				{
					y = 0;
				}
				SetConsoleCursorPosition(handle, cursor);
				printf(" ");
				if (y == Y_MAX)
				{
					cursor.Y = DONE_C.Y;
					cursor.X = DONE_C.X - 1;
				}
				else
				{
					cursor.Y = PLAYER_C.Y + 2;
					cursor.X = 1 + x * 4;
				}
				SetConsoleCursorPosition(handle, cursor);
				printf("%c", 0xAF);
				break;
			}
			case ('\r'):
			{
				if (y == 0)
				{
					if (discards[x])
					{
						--numDiscards;
						discards[x] = false;
						++cursor.Y;
						++cursor.X;
						SetConsoleCursorPosition(handle, cursor);
						printf("  ");
						--cursor.Y;
						--cursor.X;
					}
					else if (numDiscards < NUM_DISCARD)
					{
						++numDiscards;
						discards[x] = true;
						++cursor.Y;
						++cursor.X;
						SetConsoleCursorPosition(handle, cursor);
						printf("%c%c", 0xC4, 0xC4);
						--cursor.Y;
						--cursor.X;
					}
				}
				else
				{
					done = true;
					cursor.Y = PLAYER_C.Y + 3;
					cursor.X = 0;
					SetConsoleCursorPosition(handle, cursor);
					for (int card = 0; card < NUM_CARDS; ++card)
					{
						printf("    ");
					}
					cursor.Y = DONE_C.Y;
					cursor.X = DONE_C.X - 1;
					SetConsoleCursorPosition(handle, cursor);
					printf("     ");
				}
				break;
			}
			default:
			{
				break;
			}
			}
		} while (!done);
		for (int card = 0; card < NUM_CARDS; ++card)
		{
			if (discards[card])
			{
				deck.DrawCard(hand, card);
			}
		}
	}
}

void DiscardDealerCards(DECK &deck, HAND &hand)
{
	int numDiscard = rand() % (NUM_DISCARD + 1);
	bool discards[NUM_CARDS];
	int discard;
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		discards[card] = false;
	}
	for (int card = 0; card < numDiscard; ++card)
	{
		do
		{
			discard = rand() % NUM_CARDS;
		} while (discards[discard]);
		discards[discard] = true;
	}
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		if (discards[card])
		{
			deck.DrawCard(hand, card);
		}
	}
}

void DisplayHands(HAND &hand1, HAND &hand2, bool showDealer)
{
	SetConsoleCursorPosition(handle, DEALER_C);
	printf("DEALER");
	SetConsoleCursorPosition(handle, PLAYER_C);
	printf("PLAYER");
	if (showDealer)
	{
		DisplayHand(hand2, DEALER_C.Y + 2);
	}
	else
	{
		COORD cursor;
		cursor.Y = DEALER_C.Y + 2;
		cursor.X = 2;
		SetConsoleCursorPosition(handle, cursor);
		for (int card = 0; card < NUM_CARDS; ++card)
		{
			printf("XX  ");
		}
	}
	DisplayHand(hand1, PLAYER_C.Y + 2);
}

void DisplayHand(HAND &hand, int baseY)
{
	COORD cursor;
	cursor.Y = baseY;
	cursor.X = 2;
	SetConsoleCursorPosition(handle, cursor);
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		VALUE value = hand.values[card];
		SUIT suit = hand.suits[card];
		char v, s;
		switch (value)
		{
		case (TWO):
		{
			v = '2';
			break;
		}
		case (THREE):
		{
			v = '3';
			break;
		}
		case (FOUR):
		{
			v = '4';
			break;
		}
		case (FIVE):
		{
			v = '5';
			break;
		}
		case (SIX):
		{
			v = '6';
			break;
		}
		case (SEVEN):
		{
			v = '7';
			break;
		}
		case (EIGHT):
		{
			v = '8';
			break;
		}
		case (NINE):
		{
			v = '9';
			break;
		}
		case (TEN):
		{
			v = 'T';
			break;
		}
		case (JACK):
		{
			v = 'J';
			break;
		}
		case (QUEEN):
		{
			v = 'Q';
			break;
		}
		case (KING):
		{
			v = 'K';
			break;
		}
		case (ACE):
		{
			v = 'A';
			break;
		}
		default:
		{
			v = 'X';
			break;
		}
		}
		switch (suit)
		{
		case (CLUBS):
		{
			s = 'C';
			break;
		}
		case (DIAMONDS):
		{
			s = 'D';
			break;
		}
		case (HEARTS):
		{
			s = 'H';
			break;
		}
		case (SPADES):
		{
			s = 'S';
			break;
		}
		default:
		{
			s = 'X';
			break;
		}
		}
		printf("%c%c  ", v, s);
	}
}

void DisplayWinner(WINNER winner, HAND &hand1, HAND &hand2)
{
	DisplayRank(hand2, DEALER_C.Y + 3);
	DisplayRank(hand1, PLAYER_C.Y + 3);

	SetConsoleCursorPosition(handle, WINNER_C);
	switch (winner)
	{
	case (PLAYER):
	{
		printf("  YOU WIN");
		break;
	}
	case (DEALER):
	{
		printf("DEALER WINS");
		break;
	}
	case (TIE):
	{
		printf("   TIE");
		break;
	}
	default:
	{
		printf("  ERROR");
		break;
	}
	}
}

void DisplayRank(HAND &hand, int baseY)
{
	COORD cursor;
	cursor.Y = baseY;
	cursor.X = 4;
	SetConsoleCursorPosition(handle, cursor);
	switch (hand.rank)
	{
	case (ROYAL_FLUSH):
	{
		printf(" ROYAL FLUSH");
		break;
	}
	case (STRAIGHT_FLUSH):
	{
		printf("STRAIGHT FLUSH");
		break;
	}
	case (FOUR_KIND):
	{
		printf("FOUR OF A KIND");
		break;
	}
	case (FULL_HOUSE):
	{
		printf("  FULL HOUSE");
		break;
	}
	case (FLUSH):
	{
		printf("    FLUSH");
		break;
	}
	case (STRAIGHT):
	{
		printf("   STRAIGHT");
		break;
	}
	case (THREE_KIND):
	{
		printf("THREE OF A KIND");
		break;
	}
	case (TWO_PAIR):
	{
		printf("   TWO PAIR");
		break;
	}
	case (PAIR):
	{
		printf("     PAIR");
		break;
	}
	case (HIGH):
	{
		printf("  HIGH CARD");
		break;
	}
	default:
	{
		printf("     ERROR");
		break;
	}
	}
}

WINNER DetermineWinner(HAND &hand1, HAND &hand2)
{
	if (hand1.rank > hand2.rank)
	{
		return PLAYER;
	}
	else if (hand1.rank < hand2.rank)
	{
		return DEALER;
	}
	else if (hand1.primary > hand2.primary)
	{
		return PLAYER;
	}
	else if (hand1.primary < hand2.primary)
	{
		return DEALER;
	}
	else if (hand1.secondary > hand2.secondary)
	{
		return PLAYER;
	}
	else if (hand1.secondary < hand2.secondary)
	{
		return DEALER;
	}
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		if (hand1.values_sorted[card] > hand2.values_sorted[card])
		{
			return PLAYER;
		}
		else if (hand1.values_sorted[card] < hand2.values_sorted[card])
		{
			return DEALER;
		}
	}
	return TIE;
}

HAND::HAND()
{
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		values[card] = TWO;
		values_sorted[card] = TWO;
		suits[card] = CLUBS;
	}
	rank = HIGH;
	primary = TWO;
	secondary = TWO;
}

void HAND::Sort()
{
	bool ignore[NUM_CARDS];
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		ignore[card] = false;
	}
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		VALUE high;
		int i;
		for (int c = 0; c < NUM_CARDS; ++c)
		{
			if (!ignore[c])
			{
				high = values[c];
				i = c;
				break;
			}
		}
		for (int c = 0; c < NUM_CARDS; ++c)
		{
			if (values[c] > high && !ignore[c])
			{
				high = values[c];
				i = c;
			}
		}
		ignore[i] = true;
		values_sorted[card] = high;
	}
}

void HAND::DetermineRank()
{
	if (IsRoyalFlush())
	{
		rank = ROYAL_FLUSH;
	}
	else if (IsStraightFlush())
	{
		rank = STRAIGHT_FLUSH;
	}
	else if (IsFourKind())
	{
		rank = FOUR_KIND;
	}
	else if (IsFullHouse())
	{
		rank = FULL_HOUSE;
	}
	else if (IsFlush())
	{
		rank = FLUSH;
	}
	else if (IsStraight())
	{
		rank = STRAIGHT;
	}
	else if (IsThreeKind())
	{
		rank = THREE_KIND;
	}
	else if (IsTwoPair())
	{
		rank = TWO_PAIR;
	}
	else if (IsPair())
	{
		rank = PAIR;
	}
	else
	{
		rank = HIGH;
	}
}

bool HAND::IsRoyalFlush()
{
	if (IsStraightFlush() && primary == ACE)
	{
		return true;
	}
	return false;
}

bool HAND::IsStraightFlush() // XXX
{
	if (IsStraight() && IsFlush())
	{
		return true;
	}
	return false;
}

bool HAND::IsFourKind()
{
	bool good;
	for (int start = 0; start < NUM_CARDS - 3; ++start)
	{
		good = true;
		for (int card = 0; card < 3 && good; ++card)
		{
			if (values_sorted[card + start] != values_sorted[card + start + 1])
			{
				good = false;
			}
		}
		if (good)
		{
			primary = values_sorted[start];
			return true;
		}
	}
	return false;
}

bool HAND::IsFullHouse() // XXX
{
	for (int card1 = 0; card1 < NUM_CARDS - 4; ++card1)
	{
		if (values_sorted[card1] != values_sorted[card1 + 1] || values_sorted[card1] != values_sorted[card1 + 2])
		{
			continue;
		}
		for (int card2 = card1 + 3; card2 < NUM_CARDS - 1; ++card2)
		{
			if (values_sorted[card2] != values_sorted[card2 + 1])
			{
				continue;
			}
			primary = values_sorted[card1];
			secondary = values_sorted[card2];
			return true;
		}
	}
	for (int card1 = 0; card1 < NUM_CARDS - 3; ++card1)
	{
		if (values_sorted[card1] != values_sorted[card1 + 1])
		{
			continue;
		}
		for (int card2 = card1 + 2; card2 < NUM_CARDS - 2; ++card2)
		{
			if (values_sorted[card2] != values_sorted[card2 + 1] || values_sorted[card2] != values_sorted[card2 + 2])
			{
				continue;
			}
			primary = values_sorted[card1];
			secondary = values_sorted[card2];
			return true;
		}
	}
	return false;
}

bool HAND::IsFlush()
{
	int c = 0, d = 0, h = 0, s = 0;
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		switch (suits[card])
		{
		case (CLUBS):
		{
			++c;
			break;
		}
		case (DIAMONDS):
		{
			++d;
			break;
		}
		case (HEARTS):
		{
			++h;
			break;
		}
		case (SPADES):
		{
			++s;
			break;
		}
		default:
		{
			break;
		}
		}
		if (c >= 5 || d >= 5 || h >= 5 || s >= 5)
		{
			return true;
		}
	}
	return false;
}

bool HAND::IsStraight()
{
	//if (values_sorted[0] == ACE)
	//{
	//	primary = FIVE;
	//	return true;
	//}
	bool good;
	for (int start = 0; start < NUM_CARDS - 4; ++start)
	{
		good = true;
		for (int card = 0; card < NUM_CARDS - 1 && good; ++card)
		{
			if (values_sorted[card + start] != values_sorted[card + start + 1] + 1)
			{
				good = false;
			}
		}
		if (good)
		{
			primary = values_sorted[0];
			return true;
		}
	}
	return false;
}

bool HAND::IsThreeKind()
{
	bool good;
	for (int start = 0; start < NUM_CARDS - 2; ++start)
	{
		good = true;
		for (int card = 0; card < 2 && good; ++card)
		{
			if (values_sorted[card + start] != values_sorted[card + start + 1])
			{
				good = false;
			}
		}
		if (good)
		{
			primary = values_sorted[start];
			return true;
		}
	}
	return false;
}

bool HAND::IsTwoPair()
{
	for (int card1 = 0; card1 < NUM_CARDS - 3; ++card1)
	{
		if (values_sorted[card1] != values_sorted[card1 + 1])
		{
			continue;
		}
		for (int card2 = card1 + 2; card2 < NUM_CARDS - 1; ++card2)
		{
			if (values_sorted[card2] != values_sorted[card2 + 1])
			{
				continue;
			}
			primary = values_sorted[card1];
			secondary = values_sorted[card2];
			return true;
		}
	}
	return false;
}

bool HAND::IsPair()
{
	for (int card = 0; card < NUM_CARDS - 1; ++card)
	{
			if (values_sorted[card] != values_sorted[card + 1])
			{
				continue;
			}
		primary = values_sorted[card];
		return true;
	}
	return false;
}

DECK::DECK()
{
	for (int value = 0; value < NUM_VALUES; ++value)
	{
		for (int suit = 0; suit < NUM_SUITS; ++suit)
		{
			available[value][suit] = true;
		}
	}
}

void DECK::DealHand(HAND &hand)
{
	for (int card = 0; card < NUM_CARDS; ++card)
	{
		DrawCard(hand, card);
	}
}

void DECK::DrawCard(HAND &hand, int card)
{
	VALUE value;
	SUIT suit;
	do
	{
		value = (VALUE)(rand() % NUM_VALUES);
		suit = (SUIT)(rand() % NUM_SUITS);
	} while (available[value][suit] == false);
	available[value][suit] = false;
	hand.values[card] = value;
	hand.suits[card] = suit;
}
