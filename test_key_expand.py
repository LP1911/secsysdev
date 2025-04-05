import unittest, ctypes, random
from aes import AES

class TestExpandKey(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.lib = ctypes.CDLL('./rijndael.dll')
        cls.lib.expand_key.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
        cls.lib.expand_key.restype  = ctypes.POINTER(ctypes.c_ubyte)

    def call_c_expand_key(self, key: bytes) -> bytes:
        arr = (ctypes.c_ubyte * 16)(*key)
        resPtr = self.lib.expand_key(arr)
        c_expanded = ctypes.string_at(resPtr, 176)  # read 176 bytes
        return c_expanded

    def test_expand_key(self):
        for _ in range(3):
            key = bytes(random.getrandbits(8) for _ in range(16))

            # C result
            cResults = self.call_c_expand_key(key)

            # Python reference
            # Aes = 11 round keys of 4x4 int matrix
            aes_obj = AES(key)
            python_expanded = []
            # Flatten the round-key matrices into a 176-byte array
            for matrix in aes_obj._key_matrices:
                for row in matrix:
                    python_expanded.extend(row)
            pyBytes = bytes(python_expanded)

            self.assertEqual(cResults, pyBytes,
                             "expand_key did not match")

if __name__ == '__main__':
    unittest.main()