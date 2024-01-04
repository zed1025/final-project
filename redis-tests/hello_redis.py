import redis

# connection = redis.Redis()
# print(connection.ping())

r = redis.Redis(host='localhost', port=6379, decode_responses=True)

r.set('foo', 'bar')
# True
print(r.get('foo'))
# bar

