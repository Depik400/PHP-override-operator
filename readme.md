# Installation

```bash
phpize
./configure
make
make install
echo "extension=test.so" > php.ini
```
# Проект направлен на расширение возможностей интерпретатора. В данном случае это переопределение действий операторов

```php
class A
{
    public $a = 1;
    public function __add($c)
    {
        if (is_int($c))
            $this->a += $c;
        else if ($c instanceof A) {
            $this->a += $c->a;
        }
        return $this;
    }

    public function echoHere() {    
        return 'Hello world';
    }
}
$a = new A();
$b = new A();
echo ($a + $b)->echoHere(); // Prints Hello world
```
