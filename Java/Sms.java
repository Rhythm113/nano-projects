package com.emergency.rcvr;


import android.telephony.SmsManager;


//

public class Sms {
    
        public static String send(String phoneNo, String msg) {
        try {      
            SmsManager smsManager = SmsManager.getDefault();
            smsManager.sendTextMessage(phoneNo, null, msg, null, null);
            return "Sent";
        } catch (Exception ex) {
          return "Failed";
        } 
    }

}