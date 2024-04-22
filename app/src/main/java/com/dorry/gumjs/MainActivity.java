package com.dorry.gumjs;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import com.dorry.gumjs.databinding.ActivityMainBinding;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    public static boolean readFileFromAssets(Context context, String srcFile, File local) {
        int len;
        try (InputStream inputStream = context.getAssets().open(srcFile);
             FileOutputStream fos = new FileOutputStream(local);
             BufferedOutputStream bos = new BufferedOutputStream(fos)) {
            byte[] bytes = new byte[1024];
            while ((len = inputStream.read(bytes, 0, bytes.length)) != -1) {
                bos.write(bytes, 0, len);
            }
        } catch (IOException e) {
            Log.d("BHClass", "[" + srcFile + "]资源文件读取错误:" + e.getMessage());
            return false;
        }
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        Log.d("BHClass","onCreate");
        StringBuilder sb = new StringBuilder();
        try (InputStream inputStream = this.getAssets().open("com.zhiliaoapp.musically.ds")) {
            byte[] bytes = new byte[1024];
            int len;
            while ((len = inputStream.read(bytes, 0, bytes.length)) != -1) {
                sb.append(new String(bytes, 0 , len));
            }
        } catch (IOException e) {
            Log.d("BHClass", "资源文件读取错误:" + e.getMessage());
        }

        // Example of a call to a native method
        binding.button.setOnClickListener(v -> {
            System.loadLibrary("glog");
            android.util.BHHook.loadJs(sb.toString());
        });
    }
}