package ru.evgeniy.andronix;

import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.widget.Toast;

public class SparkleService extends Service {
    Process sparkleProcess;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        String abi;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            abi = Build.SUPPORTED_ABIS[0];
        } else {
            //noinspection deprecation
            abi = Build.CPU_ABI;
        }
        try{
            sparkleProcess = Runtime.getRuntime().exec(getApplicationInfo().dataDir + "/lib/libsparkle.so " + getApplicationInfo().dataDir);
        }
        catch (Exception e) { }
        Toast.makeText(this, "Service started.", Toast.LENGTH_LONG).show();
    }

    @Override
    public void onDestroy() {
        sparkleProcess.destroy();
        Toast.makeText(this, "Service stopped.", Toast.LENGTH_LONG).show();
    }
}