<?php
opcache_reset();
$a = 1;
//echo $a + 1;
//echo test();

echo multiply(1, 2, 3, 4, 5, 6, 7, 8.5, '1');

class A
{
    public $a = 1;
    // public function __add($s)
    // {
    //     return $this->a + $s;
    // }

    public function echoHere() {
        echo 'here';
    }
    public function __add($c)
    {
        if (is_int($c))
            $this->a += $c;
        else if ($c instanceof A) {
            $this->a += $c->a;
        }
        //return $this;
        return $this;
    }
    public function __pre_inc()
    {
        $this->a = 2;
        return $this;
    }
    public function __post_inc()
    {
        $this->a++;
        return $this->a;
    }
    public function __any_action($a, $b)
    {
        echo 'action - ' . $a . PHP_EOL;
        return $this;
    }
}
$b = 3;
$a = new A();
$b = new A();
//$a++;/$a;
//echo $a++;
echo ($a + $b)->echoHere();
