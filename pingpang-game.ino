#include "U8glib.h"

#define WIDTH 128
#define HEIGHT 64
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);

int paddleSize = 3;
int paddleWidth = CHAR_WIDTH * paddleSize;
int ballRadius = 2;


bool gameOver;
int paddleX;
int paddleY;
double ballX;
double ballY;
double ballAccX;
double ballAccY;

void beep(){
    analogWrite(9, 1000);
    delay(50);
    analogWrite(9, 0);
}

bool hitTest()
{
    if (ballY >= paddleY && ballY <= paddleY + ballRadius * 2)
    {
        if (ballX >= paddleX && ballX < paddleX + paddleWidth)
        {
            ballAccY = -ballAccY;
            double pos = (ballX - (paddleX)) / paddleX;
            if (pos > 0.4 && pos < 0.6)
            {
                ballAccX = random(0, 8) / 10 - 0.4;
                ballAccY;
            }
            else if (pos <= 0.4)
            {
                ballAccX = ballAccX * (1 + random(0, 8) / 10);
                ballAccY *= 1.1;
            }
            else if (pos >= 0.6)
            {
                ballAccX = ballAccX * (1 + random(0, 8) / 10);
                ballAccY *= 1.1;
            }
            Serial.println(ballAccX);
            return true;
        }
    }
    return false;
}

double renderGameOver()
{
    u8g.setFont(u8g_font_unifont);
    u8g.setFontPosTop();
    u8g.drawStr((WIDTH - 9 * CHAR_WIDTH) / 2, 22, "GAME OVER");
}

void render(void)
{
    ballX += ballAccX;
    ballY += ballAccY;
    if (hitTest())
    {
        beep();
        return;
    }
    else if (ballY > HEIGHT - ballRadius)
    {
        // GAME OVER
        gameOver = true;
    }
    else if (ballY < 0)
    {
        // Hit ceiling
        ballY = 0;
        ballAccY = -ballAccY;
        ballAccX = ballAccX;
        beep();
    }
    
    if (ballX < 0)
    {
        ballX = 0;
        ballAccX = -ballAccX;
        beep();
    }
    else if (ballX > WIDTH - ballRadius)
    {
        ballX = WIDTH - ballRadius;
        ballAccX = -ballAccX;
        beep();
    }

    if (ballAccX > 1)
    {
        ballAccX = 0.5;
    }
    else if (ballAccX < -1)
    {
        ballAccX = -0.5;
    }

    u8g.drawStr(paddleX, paddleY, "xxxx");
    u8g.drawDisc(ballX, ballY, ballRadius);
}

void reset()
{
    paddleX = 6 * CHAR_WIDTH;
    paddleY = HEIGHT - CHAR_HEIGHT;
    ballX = WIDTH / 2;
    ballY = 0;
    ballAccX = random(0, 8) / 10 - 0.8;
    ballAccY = 0.33;
    gameOver = false;
}

void setup(void)
{
    Serial.begin(9600);
    Serial.println("Hello");
    
    // Setup display
    u8g.setColorIndex(1);
    u8g.setFont(u8g_font_unifont);
    u8g.setFontPosTop();
    
    // Setup left/right key
    pinMode(2, INPUT);
    digitalWrite(2, HIGH);
    pinMode(3, INPUT);
    digitalWrite(3, HIGH);
    attachInterrupt(digitalPinToInterrupt(2), []() {
        if (!gameOver)
        {
            paddleX = constrain(paddleX - CHAR_WIDTH, 0, WIDTH - paddleWidth);
        }
        else
        {
            reset();
        }
    }, FALLING);
    attachInterrupt(digitalPinToInterrupt(3), []() {
        if (!gameOver)
        {
            paddleX = constrain(paddleX + CHAR_WIDTH, 0, WIDTH - paddleWidth);
        }
        else
        {
            reset();
        }
    }, FALLING);

    // Reset variables
    reset();
}

void loop(void)
{
    u8g.firstPage();
    do
    {
        if (!gameOver)
        {
            render();
        }
        else
        {
            renderGameOver();
        }
    }
    while(u8g.nextPage());
}

