import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

public class Androido {

    private Context context;

    public Androido(Context context) {
        this.context = context;
    }

    public void write(String data, String filePath) throws Exception {
        File file = new File(filePath);
        FileOutputStream outputStream = new FileOutputStream(file);
        outputStream.write(data.getBytes());
        outputStream.close();
    }

    public String read(String filePath) throws Exception {
        File file = new File(filePath);
        FileInputStream inputStream = new FileInputStream(file);
        byte[] bytes = new byte[(int) file.length()];
        inputStream.read(bytes);
        String data = new String(bytes);
        inputStream.close();
        return data;
    }

    public void requestPermission() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
        context.startActivity(intent);
    }
}
