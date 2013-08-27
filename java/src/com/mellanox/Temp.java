/*
** Copyright (C) 2013 Mellanox Technologies
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at:
**
** http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
** either express or implied. See the License for the specific language
** governing permissions and  limitations under the License.
**
*/
package com.mellanox;

import java.util.concurrent.*;
//import java.util.concurrent.Executors;

public class Temp {
	/*
	ExecutorService executor = Executors.newCachedThreadPool();
	
	public int runEventLoop(){return 0;}
	
	public void runWithTO (int maxEvents, int sec){
		
		
		Callable<Object> task = new Callable<Object>() {
			   public Object call() {
			      return runEventLoop();
			   }
			};
		/*	
			final Future<?> future = executor.submit(new Callable() {
			public void run() {
				try {
					runEventLoop();
				} catch (Exception e) {
					throw new RuntimeException(e);
				}
				}
			});
			*//*
			Future<Object> future = executor.submit(task);
			try {
			   Object result = future.get(sec, TimeUnit.SECONDS); 
			} catch (TimeoutException ex) {
			   // stop ev_loop and process all what is left in queue
			} catch (InterruptedException e) {
			   // handle the interrupts
			} catch (ExecutionException e) {
			   // handle other exceptions
			} finally { // future.cancel(false); // may or may not desire this
			}
	}*/
}

/*
how can I pass parameter to the blockingMethod() 
		
Create a new class called BlockingMethodCallable whose contructor 
accepts the parameters you want to pass to blockingMethod() 
and store them as member variables (probably as final). Then inside call() 
pass those parameters to the blockMethod()
*/