package com.example.webgui;

import android.os.Bundle;
import android.view.Window;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.app.Activity;
import android.view.Menu;

public class MainActivity extends Activity {

	WebView mWebView;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	final Activity mActivity = this;
    	
        super.onCreate(savedInstanceState);
        
        // Adds Progress bar Support
        this.getWindow().requestFeature(Window.FEATURE_PROGRESS);
        
        setContentView(R.layout.activity_main);
        
        // Makes Progress bar Visible
        getWindow().setFeatureInt( Window.FEATURE_PROGRESS, Window.PROGRESS_VISIBILITY_ON);

        mWebView = (WebView) findViewById( R.id.webview );
        mWebView.getSettings().setJavaScriptEnabled(true);     
        mWebView.loadUrl("http://<url to your webgui server here>");
        
        mWebView.setWebViewClient(new WebViewClient());
        /*mWebView.setWebChromeClient(new WebChromeClient() 
        {
            public void onProgressChanged(WebView view, int progress)  
            {
                //Make the bar disappear after URL is loaded, and changes string to Loading...
                mActivity.setTitle("Loading...");
                mActivity.setProgress(progress * 100); //Make the bar disappear after URL is loaded

                // Return the app name after finish loading
                if(progress == 100)
                {
                	mActivity.setTitle(R.string.app_name);
                }
            }
        });*/
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
    
}
