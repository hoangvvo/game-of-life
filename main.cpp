#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <random>

std::default_random_engine generator;

int randInt(int max)
{
  std::uniform_int_distribution<int> distribution(0, max - 1);
  return distribution(generator);
}

class Field
{
public:
  bool **s;
  int w;
  int h;

  Field(int w, int h)
  {
    this->w = w;
    this->h = h;
    this->s = new bool *[this->h];
    for (int i = 0; i < this->h; i += 1)
    {
      this->s[i] = new bool[this->w];
    }
  }
  ~Field()
  {
    for (int i = 0; i < this->h; i += 1)
    {
      delete[] this->s[i];
    }
    delete[] this->s;
  }

  // Set sets the state of the specified cell to the given value.
  void Set(int x, int y, bool b)
  {
    this->s[y][x] = b;
  }
  // Alive reports whether the specified cell is alive.
  // If the x or y coordinates are outside the field boundaries they are wrapped
  // toroidally. For instance, an x value of -1 is treated as width-1.
  bool Alive(int x, int y)
  {
    x += this->w;
    x %= this->w;
    y += this->h;
    y %= this->h;
    return this->s[y][x];
  }
  // Next returns the state of the specified cell at the next time step.
  bool Next(int x, int y)
  {
    // Count the adjacent cells that are alive.
    int alive = 0;
    for (int i = -1; i <= 1; i++)
    {
      for (int j = -1; j <= 1; j++)
      {
        if ((j != 0 || i != 0) && this->Alive(x + i, y + j))
        {
          alive++;
        }
      }
    }
    // Return next state according to the game rules:
    //   exactly 3 neighbors: on,
    //   exactly 2 neighbors: maintain current state,
    //   otherwise: off.
    return alive == 3 || alive == 2 && this->Alive(x, y);
  }
};

class Life
{
public:
  Field *a;
  Field *b;
  int w;
  int h;

  Life(int w, int h)
  {
    this->a = new Field(w, h);
    for (int i = 0; i < (w * h / 4); i++)
    {
      this->a->Set(randInt(w), randInt(h), true);
    }
    this->b = new Field(w, h),
    this->w = w;
    this->h = h;
  }

  ~Life()
  {
    delete this->a;
    delete this->b;
  }

  // Step advances the game by one instant, recomputing and updating all cells.
  void Step()
  {
    // Update the state of the next field (b) from the current field (a).
    for (int y = 0; y < this->h; y++)
    {
      for (int x = 0; x < this->w; x++)
      {
        this->b->Set(x, y, this->a->Next(x, y));
      }
    }
    // Swap fields a and b.
    Field *temp = this->a;
    this->a = this->b;
    this->b = temp;
  }

  // String returns the game board as a string.
  std::string String()
  {
    std::string buf = "";
    for (int y = 0; y < this->h; y++)
    {
      for (int x = 0; x < this->w; x++)
      {
        std::string b = " ";
        if (this->a->Alive(x, y))
        {
          b = '*';
        }
        buf += b;
      }
      buf += "\n";
    }
    return buf;
  }
};

int main()
{
  Life *l = new Life(40, 15);
  while (true)
  {
    l->Step();
    std::cout << "\033c" << l->String();
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000) / 30);
  }
}