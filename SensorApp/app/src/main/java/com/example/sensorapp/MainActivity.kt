package com.example.sensorapp

import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import com.example.userapp.Bluetooth
import java.lang.Exception
import android.content.IntentFilter

import android.view.View
import android.widget.*
import android.widget.ArrayAdapter

import android.content.BroadcastReceiver
import android.content.Context
import android.renderscript.Sampler
import kotlin.collections.ArrayList as ArrayList

import com.anychart.AnyChartView

import com.anychart.chart.common.dataentry.ValueDataEntry

import com.anychart.chart.common.dataentry.DataEntry

import com.anychart.AnyChart

val TAG = "Main"

val MESSAGE_STATE_CHANGE = 1
val MESSAGE_READ = 2
val MESSAGE_WRITE = 3
val MESSAGE_DEVICE_NAME = 4
val MESSAGE_TOAST = 5
val MESSAGE_LOSTCO = 667
val MESSAGE_FAILEDCO = 668

val TOAST_START_TIME_REPORT = 669
val TOAST_END_TIME_REPORT = 670


class MainActivity : AppCompatActivity() {

    var handler: Handler = object : Handler(Looper.myLooper()!!) {
        override fun handleMessage(msg: Message) {
            when (msg.what) {
                MESSAGE_STATE_CHANGE -> {
                    Log.d(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1)
                    btStatus("${msg.arg1}")
                }
                MESSAGE_WRITE -> Log.d(TAG, "MESSAGE_WRITE ")
                MESSAGE_READ -> {
                    Log.d(TAG, "MESSAGE_READ $msg")
                    when {
                        msg.toString().contains("DATA") -> {
                            //Begin reception of data
                            Log.d(TAG, "Receiving report data...")
                            //add linechart point
                        }
                    }

                }
                MESSAGE_DEVICE_NAME -> Log.d(TAG, "MESSAGE_DEVICE_NAME $msg")
                MESSAGE_TOAST -> {
                    Log.d(TAG, "MESSAGE_TOAST: $msg")
                }
                MESSAGE_LOSTCO -> {toastoshow(MESSAGE_LOSTCO)}
                MESSAGE_FAILEDCO -> {toastoshow(MESSAGE_FAILEDCO)}
            }
        }
    }


    @SuppressLint("ResourceType")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val btButton = findViewById<Button>(R.id.btButton)
        val displayButton = findViewById<Button>(R.id.display_button)
        val lineChartView = findViewById<AnyChartView>(R.id.line_chart)
        val lineChart = AnyChart.line()
        lineChartView.setChart(lineChart) //set pie on view

        btStatus(Bluetooth(this, handler).getState().toString())

        val data: MutableList<DataEntry> = ArrayList()
        /*
        dataButton1.setOnClickListener{
            data.add(ValueDataEntry("1",1))
            data.add(ValueDataEntry("2",2))
            data.add(ValueDataEntry("4",4))
            lineChart.data(data)
        }

        dataButton2.setOnClickListener{
            data.add(ValueDataEntry("12",8))
            data.add(ValueDataEntry("56",4))
            data.add(ValueDataEntry("54",1))
            lineChart.data(data)
        }
        */


        btButton.setOnClickListener{
            if ( Bluetooth(this, handler).getState()  == 0 ) connectService("HC-05")
            else Bluetooth(this, handler).stop()
        }

        displayButton.setOnClickListener {
            if (Bluetooth(this, handler).getState() == 3) {
                if (displayButton.text == "Start Acquisition") {
                    Bluetooth(this, handler).sendMessage("START_ACQUISITION")
                    data.clear() //Clear chart data
                    displayButton.text = "Stop Acquisition"
                } else {
                    Bluetooth(this, handler).sendMessage("STOP_ACQUISITION")
                    displayButton.text = "Start Acquisition"
                }
            } else Toast.makeText(this, "Sensor is not connected", Toast.LENGTH_SHORT).show()
        }

    }

    fun btStatus(btState: String){
        val BT_state: Int = btState.toInt()
        val btButton = findViewById<Button>(R.id.btButton)
        if (BT_state == 0) //STATE.DISCONNECTED
            btButton.text = "Sensor disconnected"
        else if (BT_state == 2) //STATE.CONNECTING
            btButton.text = "Connecting to sensor..."
        else if (BT_state == 3) //STATE.CONNECTED
            btButton.text = "Sensor connected"
        else //Undefined state
            btButton.text = "NOT DEFINED"
    }

    fun connectService(devicename: String) {
        try {
            val bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

            if (bluetoothAdapter.isEnabled) {
                Log.d(TAG, "Launch connectDevice")
                Bluetooth(this, handler).connectDevice(devicename)
                Log.d(TAG, "Btservice started - listening")
            } else {
                Log.w(TAG, "Btservice started - bluetooth is not enabled")
                Toast.makeText(this,"Please enable Bluetooth",Toast.LENGTH_SHORT).show()
            }
        } catch (e: Exception) {
            Log.e(TAG, "Unable to start bt ", e)
            Toast.makeText(this,"Unable to connect $e",Toast.LENGTH_SHORT).show()
        }
    }

    fun toastoshow(int: Int){
        when (int) {
            TOAST_START_TIME_REPORT -> Toast.makeText(this, "Receiving time report", Toast.LENGTH_SHORT).show()
            TOAST_END_TIME_REPORT -> Toast.makeText(this, "Time report received", Toast.LENGTH_SHORT).show()
            MESSAGE_LOSTCO -> Toast.makeText(this, "Distant device disconnected", Toast.LENGTH_SHORT).show()
            MESSAGE_FAILEDCO -> Toast.makeText(this, "Unable to connect to device", Toast.LENGTH_SHORT).show()
        }
    }
}