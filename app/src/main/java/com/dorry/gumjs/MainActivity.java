package com.dorry.gumjs;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.BHHook;
import android.util.Log;

import com.dorry.gumjs.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        Log.d("BHClass","onCreate");
        // Example of a call to a native method
        binding.button.setOnClickListener(v -> {
            System.loadLibrary("glog");
            BHHook.loadJs("//获取context对象\n" +
                    "function getApplicationContext() {\n" +
                    "    return Java.use(\"android.app.ActivityThread\").currentApplication().getApplicationContext();\n" +
                    "}\n" +
                    "\n" +
                    "function sharedPreferencesFun() {\n" +
                    "    Java.perform(function () {\n" +
                    "      try{\n" +
                    "        var sharedPreferences = getApplicationContext().getSharedPreferences('finsky', 0); // Replace 'my_prefs' with your preference file name\n" +
                    "        var editor = sharedPreferences.edit();\n" +
                    "        // Writing value to Shared Preferences\n" +
                    "        editor.putBoolean('auto_update_enabled', false).putBoolean(\"update_over_wifi_only\",false);\n" +
                    "        editor.apply();\n" +
                    "        Java.use(\"android.util.Log\").d(\"dd\",\"DDD\");\n" +
                    "        console.log(\"EEE\");\n" +
                    "        setTimeout(sharedPreferencesFun, 1000);\n" +
                    "    }catch (e) {\n" +
                    "  \n" +
                    "    }\n" +
                    "    });\n" +
                    "}\n" +
                    "\n" +
                    "Java.perform(function() {\n" +
                    "  var ThreadCls = Java.use('java.lang.Thread');\n" +
                    "  var FridaThreadCls = Java.registerClass({\n" +
                    "      name: 'bh.FridaThread',\n" +
                    "      superClass: ThreadCls,\n" +
                    "      methods: {\n" +
                    "          '<init>': {\n" +
                    "              returnType: 'void',\n" +
                    "              argumentTypes: [],\n" +
                    "              implementation: function () {\n" +
                    "                  this.$super.$init();\n" +
                    "                  console.log('init called');\n" +
                    "              }\n" +
                    "          },\n" +
                    "          run: [{\n" +
                    "              returnType: 'void',\n" +
                    "              argumentTypes: [],\n" +
                    "              implementation() {\n" +
                    "                sharedPreferencesFun();\n" +
                    "              }\n" +
                    "          }]\n" +
                    "      }\n" +
                    "  });\n" +
                    "  var FridaThread = FridaThreadCls.$new();\n" +
                    "  FridaThread.start();\n" +
                    "});");
        });
    }

}