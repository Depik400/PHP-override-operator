<?php
opcache_reset();
$a = 1;
echo $a + 1;
echo test();

echo multiply(1, 2, 3, 4, 5, 6, 7, 8.5, '1');

class A
{
    private $a = 1;
    // public function __add($s)
    // {
    //     return $this->a + $s;
    // }

    public function __any_action($a, $b)
    {
        echo " args - $a and $b  ";
        return 0;
    }
}

$a = new A();

echo '   ' . ($a + 1) . '    ';
