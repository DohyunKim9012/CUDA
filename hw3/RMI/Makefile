main:
	javac DBRequest.java
	jar cvf DBRequest.jar DBRequest.class
	javac -cp DBRequest.jar DBServer.java
	javac -cp DBRequest.jar DBClient.java

clean:
	rm -f *.class *.jar
