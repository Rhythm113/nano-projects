//Reversed by @Rhythm113

package ic.dec;

public class Decrypt {
    /* renamed from: a */
    public static char m85a(char c) {
        if (c >= 'a' && c <= 'z') {
            char c2 = (char) (c + 5);
            return c2 > 'z' ? (char) (c2 - 26) : c2;
        } else if (c < 'A' || c > 'Z') {
            return c;
        } else {
            char c3 = (char) (c + 5);
            return c3 > 'Z' ? (char) (c3 - 26) : c3;
        }
    }

    /* renamed from: a */
    public static String dec(String str) {
        if (str == null) {
            return null;
        }
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < str.length(); i++) {
            sb.append(m85a(str.charAt(i)));
        }
        return sb.toString();
    }
}
  /* renamed from: a */
    private String dec2(String str) {
        if (str == null || str.length() == 0) {
            return null;
        }
        int length = str.length();
        char[] cArr = new char[length];
        for (int i = 0; i < length; i++) {
            cArr[i] = (char) (str.charAt(i) ^ '*');
        }
        return new String(cArr);
    }
    
    /* renamed from: a */
    private static byte[] dec3(byte[] bArr) {
        if (bArr == null || bArr.length == 0) {
            return null;
        }
        for (int i = 0; i < bArr.length; i++) {
            bArr[i] = (byte) (bArr[i] ^ 42);
        }
        return bArr;
    }

