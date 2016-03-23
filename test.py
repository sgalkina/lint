import operator
from random import randint, random, choice
from subprocess import check_output, Popen, CalledProcessError, STDOUT, PIPE
import itertools

OPERATIONS = {
    '+': operator.add,
    '-': operator.sub,
    '*': operator.mul,
    '/': operator.div,
    '<': operator.lt,
    '==': operator.eq,
    '!=': operator.ne,
    '<=': operator.le,
    '>': operator.gt,
    '>=': operator.ge,
    '%%': operator.mod,
    'pow': operator.pow,
}

MAX_DEPTH = 10
MAX_LENGTH_NUMBER = 6
MAX_LENGTH_POWER = 1

def is_int(s):
    try:
        int(s)
        return True
    except ValueError:
        return False

def random_number_string(max_length):
    n = randint(1, max_length) - 1
    return str(randint(1, 9)) + ''.join([str(randint(0, 9)) for num in xrange(n)])

def nines_number_string(max_length):
    return '9'*randint(1, max_length)

def one_zeros_number_string(max_length):
    return '1' + '0'*(randint(1, max_length) - 1)

def generate_rpn_rec(depth):
    operations = ['+', '-', '*', '/']
    if (depth != 0 and random() > 0.7) or depth >= MAX_DEPTH:
        return random_number_string(MAX_LENGTH_NUMBER)
    return ' '.join([generate_rpn_rec(depth + 1), generate_rpn_rec(depth + 1),
        choice(operations)])

def generate_rpn():
    return generate_rpn_rec(0)

def parse_rpn(rpn):
    stack = []
    for n in rpn.split():
        if n in OPERATIONS:
            n2 = stack.pop()
            n1 = stack.pop()
            a = str(int(OPERATIONS[n](int(n1), int(n2))))
            stack.append(a)
        else:
            stack.append(n)
    assert len(stack) == 1
    return int(stack[0])

def get_binary_rpns():
    result = []
    n1 = random_number_string(MAX_LENGTH_NUMBER)
    n2 = random_number_string(MAX_LENGTH_NUMBER)
    nines = nines_number_string(MAX_LENGTH_NUMBER)
    one_zeros = one_zeros_number_string(MAX_LENGTH_NUMBER)
    p = random_number_string(MAX_LENGTH_POWER)
    numbers = {
        p: int(p),
    }
    for number in [n1, n2, nines, one_zeros, '1', '0']:
        numbers[number] = int(number)
        numbers['-' + number] = -int(number)
    for (op, n, m) in itertools.product(OPERATIONS, numbers, numbers):
        if op == 'pow':
            m = p
        result.append('{} {} {}'.format(n, m, op))
    return list(set(result))

def check_rpn(process, rpn):
    process.stdin.write(rpn + '\n')
    out = process.stdout.readline()
    try:
        p_ans = parse_rpn(rpn)
    except ZeroDivisionError:
        if 'division by zero' in out:
            return
    assert is_int(out) and (int(out) == p_ans), "RPN is {}, correct answer is {}, lint answer is {}".format(rpn, p_ans, out)

def test_rpn():
    p = Popen(["./bin/main"], stdin=PIPE, stdout=PIPE)
    for rpn in get_binary_rpns():
        check_rpn(p, rpn)
    for _ in xrange(1000):
        check_rpn(p, generate_rpn())

def test_lint():
    test_rpn()
    print 'OK'

test_lint()
