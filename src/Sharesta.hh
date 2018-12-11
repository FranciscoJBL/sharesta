<?hh // strict

namespace Usox\Sharesta;

use type Facebook\HackRouter\HttpMethod;
use namespace Facebook\Experimental\Http\Message;
use namespace HH\Lib\Experimental\IO;

final class Sharesta {

    private vec<Route> $routes = vec[];

    public function __construct(
        private Message\ServerRequestInterface $server_request
    ) {
    }

    public function registerRoute(
        HttpMethod $method,
        string $path,
        ResponderType $responder
    ): void {
        $this->routes[] = new Route(
            $method,
            $path,
            $responder
        );
    }

    public function route(): Message\ResponseInterface {
        $router = new Router($this->routes);

        list($responder, $params) = $router->routeRequest($this->server_request);

        $code = 200;
        $result = '';

        try {
            $result = $responder($params);
        } catch (\Exception $exception) {
            $code = 500;
        }
        $response = new \Usox\HackTTP\Response(
            IO\request_output(),
            $code,
            '',
            dict[
                'Content-type' => vec['application/json'],
            ]
        );

        $response->getBody()->rawWriteBlocking(\json_encode($result));

        return $response;
    }
}