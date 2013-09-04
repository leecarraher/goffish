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

package edu.usc.goffish.gofs.tools.deploy;

import java.io.*;

import edu.usc.goffish.gofs.graph.*;

public interface IGraphLoader {

	/**
	 * This method should load and return a graph suitable for partitioning.
	 * 
	 * @return a graph suitable for partitioning
	 */
	public IIdentifiableVertexGraph<? extends IIdentifiableVertex, ? extends IEdge> loadGraph() throws IOException;
}
