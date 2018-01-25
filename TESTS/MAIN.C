main()
{
  int x;
  char v;
  foo = 3;
  bar = foo;
  foo += 5;
  foo += foo;
  foo -= 5;
  foo -= foo;
  foo *= 5;
  foo *= foo;
  foo /= 5;
  foo /= foo;
  foo++;
  foo--;
  foo <<= 3;
  foo <<= bar;
  foo >>= 3;
  foo >>= bar;
	asm("setq x, 5");
	asm("setq y, 5");
	asm("add x, y");
}
