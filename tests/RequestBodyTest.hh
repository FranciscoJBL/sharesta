<?hh // strict
namespace Usox\Sharesta;

use function Facebook\FBExpect\expect;
use HH\Lib\{Str, Math};
use Facebook\TypeAssert\UnsupportedTypeException;

class InputStreamWrapper {

	public static ?InputStreamWrapper $singleton;

	private int $index = 0;

	private string $data = '';

	public function read(int $count): string {
		$length = Math\minva($count, Str\length($this->data) - $this->index);
		$data = Str\slice($this->data, $this->index);
		$this->index = $this->index + $length;
		return $data;
	}

	public function eof(): bool {
		return $this->index >= Str\length($this->data) ? true : false;
	}

	public function write(string $data): void {
		$this->index = 0;
		$this->data = $data;
	}

	public function stream_open(
		string $path,
		string $mode,
		int $options,
		?string $opened_path
	): bool {
		return true;
	}

	public function stream_close(): void {
	}

	public function stream_stat(): array<string, string> {
		return array();
	}

	public function stream_flush(): bool {
		return true;
	}

	private function getSingleton(): InputStreamWrapper {
		if (static::$singleton === null) {
			static::$singleton = new InputStreamWrapper();
		}
		return static::$singleton;
	}

	public function stream_read(int $count): string {
		return $this->getSingleton()->read($count);
	}

	public function stream_eof(): bool {
		return $this->getSingleton()->eof();
	}

	public function stream_write(string $data): void {
		$this->getSingleton()->write($data);
	}

	public function unlink(): void {
	}
}

class RequestBodyTest extends \Facebook\HackTest\HackTest {

	private ?RequestBody $request_body;

	private function setUp(): void {
		\stream_wrapper_unregister('php');
		\stream_wrapper_register('php', InputStreamWrapper::class);
		$this->request_body = new RequestBody();
	}

	public function testWithoutJsonDataThrowsException(): void {
		$this->setUp();

		\file_put_contents('php://input', 'absolutly-no-json');

		expect(
			function () {
				$this->request_body?->getBody();
			}
		)
		->toThrow(
			Exception\RequestException::class
		);
	}

	public function testWithEmptyBodyReturnsEmptyDict(): void {
		$this->setUp();

		expect($this->getRequestBody([]))->toBeSame(dict([]));
	}

	public function testWithJsonDataReturnsDict(): void {
		$this->setUp();
		$request_data = ['data' => 'really'];
		$data = $this->getRequestBody($request_data);

		expect($data)
		->toBeSame(dict($request_data));
	}

	public function testGetAsStringFailsWithNonString(): void {
		$this->fillBody(['data' => 123]);

		expect(
			() ==> $this->request_body?->getAsString('data')
		)
		->toThrow(
			Exception\Request\InvalidRequestParamException::class
		);
	}

	public function testGetAsStringReturnsString(): void {
		$expectation = 'some-string';

		$this->fillBody(['data' => $expectation]);

		expect(
			$this->request_body?->getAsString('data')
		)
		->toBeSame($expectation);
	}

	public function testGetAsIntFailsWithNonInt(): void {
		$this->fillBody(['data' => 'we-want-snoo-snoo']);

		expect(
			function() {
				$this->request_body?->getAsInt('data');
			}
		)
		->toThrow(
			Exception\Request\InvalidRequestParamException::class
		);
	}

	public function testGetAsIntReturnsInt(): void {
		$expectation = 123;

		$this->fillBody(['data' => $expectation]);

		expect(
			$this->request_body?->getAsInt('data')
		)
		->toBeSame($expectation);
	}

	public function testGetAsIntFailsWithNonBool(): void {
		$this->fillBody(['data' => 'mister-noob-noob']);

		expect(
			function() {
				$this->request_body?->getAsBool('data');
			}
		)
		->toThrow(
			Exception\Request\InvalidRequestParamException::class
		);
	}

	public function testGetAsBoolReturnsBool(): void {
		$expectation = true;

		$this->fillBody(['data' => $expectation]);

		expect(
			$this->request_body?->getAsBool('data')
		)
		->toBeSame($expectation);
	}

	public function testGetAsVecThrowsExceptionOnNonArray(): void {
		$this->fillBody(['data' => 'BOOM']);

		expect(() ==> $this->request_body?->getAsVec('data'))
		->toThrow(
			Exception\Request\InvalidRequestParamException::class
		);
	}

	public function testGetAsVecReturnsVector(): void {
		$data = ['I-will-be-ignored' => 'Me-not'];

		$this->fillBody(['data' => $data]);

		expect(
			$this->request_body?->getAsVec('data')
		)
		->toBeSame(vec($data));
	}

	public function testGetAsDictThrowsExceptionOnNonArray(): void {
		$this->fillBody(['data' => 'BOOM']);

		expect(
			function () {
				$this->request_body?->getAsDict('data');
			}
		)
		->toThrow(
			Exception\Request\InvalidRequestParamException::class
		);
	}

	public function testGetAsDictReturnsDict(): void {
		$data = ['da-key' => 'da-value'];

		$this->fillBody(['data' => $data]);

		expect(
			$this->request_body?->getAsDict('data')
		)
		->toBeSame(dict($data));
	}

	private function getRequestBody(array<string, mixed>$input): ?dict<string, mixed> {
		$this->fillBody($input);

		return $this->request_body?->getBody();	
	}

	private function fillBody(array<string, mixed> $body): void {
		$this->setUp();
		\file_put_contents('php://input', \json_encode($body));
	}
}
