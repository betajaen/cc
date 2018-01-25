tests()
{
  int foo;
  foo = 5;
}

tests2()
{
  int foo;
  foo = 5;
}

main()
{
  int foo;
  char bar;
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
