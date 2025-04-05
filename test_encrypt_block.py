import unittest, ctypes, random
from aes import AES

class TestAESBlock(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.lib = ctypes.CDLL('./rijndael.dll')
        # Function signatures for block enc and decryption
        cls.lib.aes_encrypt_block.argtypes = [
            ctypes.POINTER(ctypes.c_ubyte),  # Plaintext
            ctypes.POINTER(ctypes.c_ubyte)   # Key
        ]
        cls.lib.aes_encrypt_block.restype = ctypes.POINTER(ctypes.c_ubyte)
        
        cls.lib.aes_decrypt_block.argtypes = [
            ctypes.POINTER(ctypes.c_ubyte),  # Ciphertext
            ctypes.POINTER(ctypes.c_ubyte)   # Key
        ]
        cls.lib.aes_decrypt_block.restype = ctypes.POINTER(ctypes.c_ubyte)

    def call_c_encrypt_block(self, plaintext: bytes, key: bytes) -> bytes:
        # Create the C arrays from the plaintext and key each 16 bytes
        ptArr = (ctypes.c_ubyte * 16)(*plaintext)
        keyArr = (ctypes.c_ubyte * 16)(*key)
        # Call the C func which returns a pointer to 16 bytes
        resPtr = self.lib.aes_encrypt_block(ptArr, keyArr)
        # Take the 16 bytes from the pointer
        result = ctypes.string_at(resPtr, 16)
        return result

    def call_c_decrypt_block(self, ciphertext: bytes, key: bytes) -> bytes:
        ctArr = (ctypes.c_ubyte * 16)(*ciphertext)
        keyArr = (ctypes.c_ubyte * 16)(*key)
        resPtr = self.lib.aes_decrypt_block(ctArr, keyArr)
        result = ctypes.string_at(resPtr, 16)
        return result

    def test_encrypt_block(self):
        # Test encryption on 3 random blocks
        for _ in range(3):
            plaintext = bytes(random.getrandbits(8) for _ in range(16))
            key = bytes(random.getrandbits(8) for _ in range(16))
            cCipher = self.call_c_encrypt_block(plaintext, key)
            pyCipher = AES(key).encrypt_block(plaintext)
            self.assertEqual(cCipher, pyCipher, "the AES encryption block did not match")

    def test_decrypt_block(self):
        # Test decryption on 3 random blocks
        for _ in range(3):
            plaintext = bytes(random.getrandbits(8) for _ in range(16))
            key = bytes(random.getrandbits(8) for _ in range(16))
            # Encrypt with Python reference
            pyCipher = AES(key).encrypt_block(plaintext)
            cPlain = self.call_c_decrypt_block(pyCipher, key)
            self.assertEqual(cPlain, plaintext, "The AES decryption block did not match")

if __name__ == '__main__':
    unittest.main()
