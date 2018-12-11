<?hh // strict
namespace Usox\Sharesta\Example;

require_once '../../vendor/hh_autoload.php';

use Usox\Sharesta\ApiFactory;
use Usox\Sharesta\Sharesta;
use type Facebook\HackRouter\BaseRouter;
use type Facebook\HackRouter\HttpMethod;
use type Usox\HackTTP\ServerRequest;

/**
final class RouterExample extends BaseRouter<ResponderType> {

	<<__Override>>
	protected function getRoutes(
	): ImmMap<HttpMethod, ImmMap<string, ResponderType>> {
		return ImmMap {
			HttpMethod::GET => ImmMap {
				'/' => ($_params) ==> 'Welcome home',
				'/user/{user_name}' => ($params) ==>
					'Welcome home, '.$params['user_name'],
			},
		};
	}
}
 */

final class SampleRouteHandler implements \JsonSerializable {

	public function __construct(
		private ImmMap<string, string> $params
	) {
	}

	public function jsonSerialize(): mixed {
		return 'Welcome home';
	}
}

<<__Entrypoint>>
function main(): noreturn {
	$server_request = ServerRequest::createFromGlobals();
	$sharesta = new Sharesta($server_request);

	$sharesta->registerRoute(
		HttpMethod::GET,
		'/',
		($params) ==> new SampleRouteHandler($params)
	);

	$response = $sharesta->route();

	die(0);
}
