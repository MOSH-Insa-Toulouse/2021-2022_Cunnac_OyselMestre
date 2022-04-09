package com.example.sensorapp

import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.*
import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import com.example.userapp.Bluetooth
import java.lang.Exception

import android.view.View
import android.widget.*
import android.widget.ArrayAdapter

import android.renderscript.Sampler
import androidx.core.text.isDigitsOnly
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

var inc = 0
val lineChart = AnyChart.line()
var data_array = arrayListOf<String>()
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
                    data_write(msg.obj.toString(), inc)
                    inc += 1
                    lineChart.data(data)
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

    var data: MutableList<DataEntry> = ArrayList()


    @SuppressLint("ResourceType")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val btButton = findViewById<Button>(R.id.btButton)
        val displayButton = findViewById<Button>(R.id.display_button)
        val exportButton = findViewById<Button>(R.id.savedataButton)
        val helpButton = findViewById<Button>(R.id.tuto_button)

        val lineChartView = findViewById<AnyChartView>(R.id.line_chart)

        lineChart.xScale("continuous")
        lineChart.title("Sensor Resistance (Ohms)")
        lineChartView.setChart(lineChart) //set pie on view

        btStatus(Bluetooth(this, handler).getState().toString())


        btButton.setOnClickListener{
            if ( Bluetooth(this, handler).getState()  == 0 ) connectService("HC-05")
            else Bluetooth(this, handler).stop()
        }

        displayButton.setOnClickListener {
            if (Bluetooth(this, handler).getState() == 3) {
                if (displayButton.text == "Start Acquisition") {
                    Bluetooth(this, handler).sendMessage("START_ACQUISITION")
                    inc = 0 //reset increment
                    data.clear() //Clear chart data
                    data_array.clear() //Clear data array
                    lineChart.data(data)
                    displayButton.text = "Stop Acquisition"
                } else {
                    Bluetooth(this, handler).sendMessage("STOP_ACQUISITION")
                    displayButton.text = "Start Acquisition"
                }
            } else Toast.makeText(this, "Sensor is not connected", Toast.LENGTH_SHORT).show()
        }
        exportButton.setOnClickListener{
            data_send()
        }
        helpButton.setOnClickListener{
            HelpPopup(this).show()
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

    fun data_write(message: String, inc: Int){
        Log.d(TAG, "MESSAGE:$message, INC: $inc")
        Log.d(TAG, "MESSAGE LENGTH: ${message.length}")
        if (message != null) {
            val subseq = message.subSequence(0, message.length - 3).toString()
            if (subseq.isDigitsOnly()) {
                Log.d(TAG, "APPEND DATA: $message")
                data.add(ValueDataEntry(inc, subseq.toInt()))
                data_array.add(subseq)
            }
        }
    }

    fun data_send() {

        Log.i(TAG, "data send FUNCTION")

        if (data_array.isEmpty()) Toast.makeText(this, "No data to export !", Toast.LENGTH_SHORT).show()
        else {
        val sendIntent = Intent(Intent.ACTION_VIEW)

            var stringToSend = ""

            for (data_element in data_array){ //Make text to send
                stringToSend += "$data_element\n"
            }
            Log.i(TAG, "Data to send: $stringToSend")

            sendIntent.putExtra(Intent.EXTRA_TEXT, stringToSend) //text to write

            try {
                startActivity(Intent.createChooser(sendIntent, "Send data..."))
                Log.i(TAG, "BONJOUR")
            } catch (ex: ActivityNotFoundException) {
                Toast.makeText(
                    this,
                    "Error, oups...", Toast.LENGTH_SHORT
                ).show()
            }
        }
    }
}