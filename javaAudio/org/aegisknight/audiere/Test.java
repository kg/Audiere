import org.aegisknight.audiere.Context;
import org.aegisknight.audiere.Stream;

public class Test
{
    public static void main(String[] args) {
	try {
	    Context context = new Context("autodetect", "");
	    String name = "M:/mpeg/The Cure/Wish/01 - Open.ogg";
	    Stream stream = context.openStream(name);
	    stream.play();
	    Thread.currentThread().sleep(10000);
	}
	catch (Exception e) {
	    System.out.println(e);
	}
    }
}
