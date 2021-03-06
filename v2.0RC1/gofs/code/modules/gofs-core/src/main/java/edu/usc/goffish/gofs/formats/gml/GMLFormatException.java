/*
 *    Copyright 2013 University of Southern California
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License. 
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

package edu.usc.goffish.gofs.formats.gml;

public class GMLFormatException extends RuntimeException {

	private static final long serialVersionUID = 2502818346612789984L;

	public GMLFormatException() {
		super();
	}

	public GMLFormatException(String message) {
		super(message);
	}

	public GMLFormatException(Throwable cause) {
		super(cause);
	}

	public GMLFormatException(String message, Throwable cause) {
		super(message, cause);
	}
}
