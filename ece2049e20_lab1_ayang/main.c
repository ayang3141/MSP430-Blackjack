/************** ECE2049 DEMO CODE ******************/
/************** 14 May 2018   **********************/
/***************************************************/

#include <msp430.h>

#include "peripherals.h"
#include <stdlib.h>
#include "utils/debug_assert.h"

// Include puzzles for lab 0

// Function Prototypes
enum State {welcome_screen, shufflingAndDealing, human_turn, dealer_turn, endgame} state;
enum state2 {human, dealer, tie};
enum state2 winner;
char computeRank(int x);
char computeSuit(int x);
void dealHumanCard(void);
void dealDealerCard(void);
int isUsedCard(int card, int array[8]);
void display_card(char suit, char rank, int x, int y);
int computeScore(char rank_array[4]);
void determineWinner(void);
void clearHands(void);




// Declare globals here
int shufflecount = 0;


int human_score, dealer_score, numUsedCards, numHumanCards, numDealerCards;
char button_state;
int used_cards[8];

// Maximum number of cards each player can have is 4
char human_suit[4];
char human_rank[4];
char dealer_suit[4];
char dealer_rank[4];


int deck[52] = {0x4841, 0x4832, 0x4833, 0x4834, 0x4835, 0x4836, 0x4837, 0x4838, 0x4839, 0x4858, 0x484A, 0x4851, 0x484B,
                0x4341, 0x4332, 0x4333, 0x4334, 0x4335, 0x4336, 0x4337, 0x4338, 0x4339, 0x4358, 0x434A, 0x4351, 0x434B,
                0x4441, 0x4432, 0x4433, 0x4434, 0x4435, 0x4436, 0x4437, 0x4438, 0x4439, 0x4458, 0x444A, 0x4451, 0x444B,
                0x5341, 0x5332, 0x5333, 0x5334, 0x5335, 0x5336, 0x5337, 0x5338, 0x5339, 0x5358, 0x534A, 0x5351, 0x534B};
// Byte 0 position: 41 = Ace;  32 = 2; 33 = 3; 34 = 4; 35 = 5; 36 = 6; 37 = 7; 38 = 8; 39 = 9; 58 = X (10); 4A = J; 51 = Q; 4B = K
// Byte 1 position: 48 = H; 43 = C; 44 = D; 53 = S

// Main
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                 // You can then configure it properly, if desired

    // *** System initialization ***
    initLaunchpadButtons();
    initLaunchpadLeds();
    configDisplay();

    // ********* DEMO CODE **********
    // This part of the program demonstrates an actual
    // program that provides some useful functionality with the hardware

    // *** Intro Screen ***

    int h_center = LCD_HORIZONTAL_MAX / 2;
    Graphics_clearDisplay(&g_sContext); // Clear the display

    // Write some text to the display
    //int h_center = LCD_HORIZONTAL_MAX / 2;
    //Graphics_drawStringCentered(&g_sContext, "BLACKJACK",  AUTO_STRING_LENGTH, h_center, 15, TRANSPARENT_TEXT);


    // We are now done writing to the display.  However, if we stopped here, we would not
    // see any changes on the actual LCD.  This is because we need to send our changes
    // to the LCD, which then refreshes the display.
    // Since this is a slow operation, it is best to refresh (or "flush") only after
    // we are done drawing everything we need.
    Graphics_flushBuffer(&g_sContext);

    state = welcome_screen;



    while (1)    // Forever loop
    {
        // right button_state (S2) has value 1 when pressed
        // left button_state (S1) has value 2 when pressed
        // button_state = readLaunchpadButtons();

        switch(state)
        {
            case welcome_screen:    // Display the welcome screen
            {
                Graphics_drawStringCentered(&g_sContext, "MSP430 Blackjack",  AUTO_STRING_LENGTH, h_center, 15, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Press a button",  AUTO_STRING_LENGTH, h_center, 25, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "to start",  AUTO_STRING_LENGTH, h_center, 35, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);

                if(readLaunchpadButtons() != 0)   // A button (S1 or S2) is pressed
                {
                    state = shufflingAndDealing;

                    // Clears data from previous games
                    human_score = 0;
                    dealer_score = 0;
                    numUsedCards = 0;
                    numHumanCards = 0;
                    numDealerCards = 0;
                    winner = tie;
                    shufflecount = 0;


                    Graphics_clearDisplay(&g_sContext); // Clear the display
                }

                break;
            }
            case shufflingAndDealing:
            {

                Graphics_drawStringCentered(&g_sContext, "Shuffling",  AUTO_STRING_LENGTH, h_center, 15, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Press S1",  AUTO_STRING_LENGTH, h_center, 25, OPAQUE_TEXT);

                unsigned char str[4];   // Displays the value of shufflecount on the LCD screen
                str[0] = '<';
                str[1] = shufflecount + '0';
                str[2] = '>';
                str[3] = '\0';

                Graphics_drawStringCentered(&g_sContext, str,  AUTO_STRING_LENGTH, h_center, 35, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);


                if(readLaunchpadButtons() == 2) // The S1 Button is pressed
                {
                    srand(shufflecount);    // uses shufflecount as seed for rand(), i.e. "shuffles the deck"

                    // deals the human player two cards
                    dealHumanCard();
                    dealHumanCard();

                    // deals the dealer player one card
                    dealDealerCard();

                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    state = human_turn;
                    break;
                }

                if (shufflecount == 9)  // loops the value of shufflecount from 0-9
                {
                    shufflecount = 0;
                }
                else
                {
                    shufflecount++;
                }



                break;
            }
            case human_turn:
            {
                Graphics_drawStringCentered(&g_sContext, "Player's turn",  AUTO_STRING_LENGTH, h_center, 15, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Player",  AUTO_STRING_LENGTH, 32, 35, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "CPU",  AUTO_STRING_LENGTH, 96, 35, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);
                P4OUT |= BIT7;      // Green LED is on to indicate it is Player's turn


                // Display the Player and Dealer hands
                int i = 0;
                int j = 0;
                while(i < numHumanCards)        // Displays Player hand
                {
                    display_card(human_suit[i], human_rank[i], 32, 45 + i*10);
                    i += 1;
                }
                Graphics_flushBuffer(&g_sContext);
                while(j < numDealerCards)       // Displays Dealer hand
                {
                    display_card(dealer_suit[j], dealer_rank[j], 96, 45 + j*10);
                    j += 1;
                }
                Graphics_flushBuffer(&g_sContext);


                if(computeScore(human_rank) > 21) // Checks if Player score exceeds 21, if so, Player loses
                {
                    state = endgame;    // Game ends
                    P4OUT &= ~BIT7;
                    human_score = computeScore(human_rank);
                    determineWinner();
                    Graphics_clearDisplay(&g_sContext); // Clear the display
                }


                if((readLaunchpadButtons() == 1) || (numHumanCards == 4)) // S2 is pressed ==> STAY
                {
                    state = dealer_turn;
                    human_score = computeScore(human_rank);
                    P4OUT &= ~BIT7;
                    Graphics_clearDisplay(&g_sContext); // Clear the display
                }
                else if(readLaunchpadButtons() == 2) // S1 is pressed ==> HIT
                {
                    // deals the Player 1 card
                    dealHumanCard();
                }

                break;
            }
            case dealer_turn:
            {
                Graphics_drawStringCentered(&g_sContext, "Dealer's turn",  AUTO_STRING_LENGTH, h_center, 15, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Player",  AUTO_STRING_LENGTH, 32, 35, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "CPU",  AUTO_STRING_LENGTH, 96, 35, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);
                P1OUT |= BIT0;      // Red LED is on to indicate it is Dealer's turn

                // Display the Player and Dealer hands
                int i = 0;
                int j = 0;
                while(i < numHumanCards)        // Displays Player hand
                {
                    display_card(human_suit[i], human_rank[i], 32, 45 + i*10);
                    i += 1;
                }
                Graphics_flushBuffer(&g_sContext);
                while(j < numDealerCards)       // Displays Dealer hand
                {
                    display_card(dealer_suit[j], dealer_rank[j], 96, 45 + j*10);
                    j += 1;
                }
                Graphics_flushBuffer(&g_sContext);

                // Remember that passing an array through a function only requires the NAME of array!
                dealer_score = computeScore(dealer_rank);

                if((dealer_score >= 17) || (numDealerCards == 4)) // Dealer's Turn ENDS
                {
                    state = endgame;
                    P1OUT &= ~BIT0;
                    // dealer_score is previously calculated
                    determineWinner();
                    Graphics_clearDisplay(&g_sContext); // Clear the display
                }
                else if(dealer_score < 17) // Dealer HITS
                {
                    // deals Dealer 1 card
                    dealDealerCard();
                }

                break;
            }
            case endgame:
            {
                switch(winner)
                {
                    case human:
                    {
                        // Code for human win
                        Graphics_drawStringCentered(&g_sContext, "You Win!",  AUTO_STRING_LENGTH, h_center, 25, OPAQUE_TEXT);
                        Graphics_drawStringCentered(&g_sContext, "Congratulations!",  AUTO_STRING_LENGTH, h_center, 35, OPAQUE_TEXT);
                        Graphics_flushBuffer(&g_sContext);

                        P4OUT = P4OUT ^ BIT7;   // Green LED blinks to show Player has won!



                        break;
                    }
                    case dealer:
                    {
                        // Code for dealer win
                        Graphics_drawStringCentered(&g_sContext, "You Lose!",  AUTO_STRING_LENGTH, h_center, 25, OPAQUE_TEXT);
                        Graphics_drawStringCentered(&g_sContext, "Too Bad!",  AUTO_STRING_LENGTH, h_center, 35, OPAQUE_TEXT);
                        Graphics_flushBuffer(&g_sContext);

                        P1OUT = P1OUT ^ BIT0;   // Red LED blinks to show Player has lost

                        break;
                    }
                    case tie:
                    {
                        // Code for tie
                        Graphics_drawStringCentered(&g_sContext, "TIE",  AUTO_STRING_LENGTH, h_center, 25, OPAQUE_TEXT);
                        Graphics_drawStringCentered(&g_sContext, "Play Again!",  AUTO_STRING_LENGTH, h_center, 35, OPAQUE_TEXT);
                        Graphics_flushBuffer(&g_sContext);



                        break;
                    }
                } // end winner switch statement



                Graphics_drawStringCentered(&g_sContext, "Press a Button",  AUTO_STRING_LENGTH, h_center, 55, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "to return to ",  AUTO_STRING_LENGTH, h_center, 65, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Start",  AUTO_STRING_LENGTH, h_center, 75, OPAQUE_TEXT);


                if(readLaunchpadButtons() != 0)
                {
                    state = welcome_screen;
                    P1OUT &= ~BIT0;
                    P4OUT &= ~BIT7;
                    clearHands();
                    Graphics_clearDisplay(&g_sContext); // Clear the display

                    break;

                }

                break;
            }   // end endgame state

        } // end switch statement



    }  // end while (1)
}   // end main loop

char computeRank(int x)
{
    return (0x00FF & x);
}


char computeSuit(int x)
{
    int byteone = 0xFF00 & x;
    return (byteone >> 8);
}

void dealHumanCard(void)
{
    int random = deck[rand() % 52];    // selects a random card from the deck of 52 cards
    while(isUsedCard(random, used_cards))
    {
        random = deck[rand() % 52];
    }

    used_cards[numUsedCards] = random;
    numUsedCards += 1;

    human_suit[numHumanCards] = computeSuit(random);
    human_rank[numHumanCards] = computeRank(random);
    numHumanCards += 1;
}

void dealDealerCard(void)
{

    int random = deck[rand() % 52];    // selects a random card from the deck of 52 cards
    while(isUsedCard(random, used_cards))
    {
        random = deck[rand() % 52];
    }

    used_cards[numUsedCards] = random;
    numUsedCards += 1;

    dealer_suit[numDealerCards] = computeSuit(random);
    dealer_rank[numDealerCards] = computeRank(random);
    numDealerCards += 1;
}



int isUsedCard(int card, int array[8]) // array must have length 8
{
    int i;
    for(i = 0; i < numUsedCards; i++)
    {
        if(array[i] == card)
        {
            return 1;
        }
    }
    return 0;
}

void display_card(char suit, char rank, int x, int y)
{
    unsigned char str[4];
    str[0] = suit;
    str[1] = '-';
    str[2] = rank;
    str[3] = '\0';

    Graphics_drawStringCentered(&g_sContext, str,  AUTO_STRING_LENGTH, x, y, OPAQUE_TEXT);
}

int computeScore(char rank_array[4])
{
    int score = 0;
    int i;
    int numAces = 0;
    char rank;
    for(i = 0; i < 4; i++)
    {
        rank = rank_array[i];
        if(rank == 'A')
        {
            numAces += 1;
        }
        else if(rank == '2')    // 2
        {
            score += 2;
        }
        else if(rank == '3')    // 3
        {
            score += 3;
        }
        else if(rank == '4')    // 4
        {
            score += 4;
        }
        else if(rank == '5')    // 5
        {
            score += 5;
        }
        else if(rank == '6')    // 6
        {
            score += 6;
        }
        else if(rank == '7')    // 7
        {
            score += 7;
        }
        else if(rank == '8')    // 8
        {
            score += 8;
        }
        else if(rank == '9')    // 9
        {
            score += 9;
        }
        else if((rank == 'X') || (rank == 'J') || (rank == 'Q') || (rank == 'K'))       // 10, Jack, Queen, or King
        {
            score += 10;
        }
    }

    for(i = 0; i < numAces; i++)
    {
        if((score + 11) <= 21)
        {
            score = score + 11;
        }
        else
        {
            score = score + 1;
        }
    }

    return score;
}

void determineWinner(void)
{
    // Winning through BUST
    if (human_score > 21)
    {
        winner = dealer;
    }
    else if (dealer_score > 21)
    {
        winner = human;
    }
    else    // Winning via greater score
    {
        if (human_score > dealer_score)
        {
            winner = human;
        }
        else if (human_score < dealer_score)
        {
            winner = dealer;
        }
        else
        {
            winner = tie;
        }
    }
}

void clearHands(void)
{

    int i;
    for(i = 0; i < 4; i++)  // Clears our player and dealer hands
    {
        human_suit[i] = '\0';
        human_rank[i] = '\0';
        dealer_suit[i] = '\0';
        dealer_rank[i] = '\0';
    }

    for(i = 0; i < 8; i++)  // Clears out used cards
    {
        used_cards[i] = 0x0000;
    }
}









