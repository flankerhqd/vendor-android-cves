package com.samsung.SMT.lang.poc;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.os.Bundle;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ///data/app/com.samsung.SMT.lang.poc-ILVI78QqQZJs35VkUAQajA==/lib/arm
       // System.loadLibrary("mstring");

        Intent intent = new Intent();
        intent.setComponent(new ComponentName("com.samsung.SMT", "com.samsung.SMT.SamsungTTSService"));
        startService(intent);


    }
}
