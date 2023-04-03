import random
from math import gcd, sqrt


# Check if the number is prime
def is_prime(number: int) -> bool:
    if number % 2 == 0:
        return False
    if number <= 2:
        return False
    for i in range(3, int(sqrt(number))+1, 2):
        if number % i == 0:
            return False
    return True


# Generating two large prime numbers
def generate_prime_numbers() -> tuple:
    p = random.randint(38475, 125631)
    q = random.randint(38475, 125631)
    while not is_prime(p):
        p = random.randint(38475, 125631)
    while not is_prime(q):
        q = random.randint(38475, 125631)
    return p, q


# Calculating the product of p and q (n):
def product(p: int, q: int) -> int:
    n = p*q
    return n


# Calculating the totient of n
def totient_of_n(p: int, q: int) -> int:
    tot_of_n = (p - 1) * (q - 1)
    return tot_of_n


# Finding co-prime number to tot_of_n:
def finding_coprime(first_number: int) -> int:
    e = random.randint(1, first_number)
    while gcd(e, first_number) != 1:
        e = random.randint(1, first_number)
    return e  # public key


# Calculating d (the private key exponent)
def calculating_d(e: int, tot_of_n: int) -> int:
    d = 1
    while (d*e) % tot_of_n != 1:
        d += 1
    return d


# Encrypting
def encrypt(public_key: tuple, plaintext: str) -> list:
    e, n = public_key
    cipher = list()
    for char in plaintext:
        a = ord(char)
        cipher.append(pow(a, e, n))
    return cipher


# Decrypting
def decrypt(private_key: tuple, cipher: list) -> str:
    decrypted, n = private_key
    plaintext = ''
    for char in cipher:
        a = pow(char, decrypted, n)
        plaintext = plaintext + str(chr(a))
    return plaintext


def main():
    p, q = generate_prime_numbers()
    n = product(p, q)
    tot_of_n = totient_of_n(p, q)
    e = finding_coprime(tot_of_n)
    d = calculating_d(e, tot_of_n)

    message = input("Please enter text to cipher it: ")
    print("The original text is: %s" % message)
    cipher = encrypt((e, n), message)
    print("Cipher is: %s" % cipher)
    original_text = decrypt((d, n), cipher)
    print("Decrypted text is: %s" % original_text)


if __name__ == "__main__":
    main()
