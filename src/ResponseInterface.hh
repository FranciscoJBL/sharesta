<?hh // strict
namespace Usox\Sharesta;

interface ResponseInterface {

	public function send(int $code, mixed $body = ''): void;
}
