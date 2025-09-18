package resources;

import com.diogonunes.jcdp.color.api.Ansi.Attribute;
import com.diogonunes.jcdp.color.api.Ansi.BColor;
import com.diogonunes.jcdp.color.api.Ansi.FColor;

public class TestColor {

	public static void main(String[] args) {
		System.out.printf("Hello console", Attribute.BOLD , FColor.RED);
	}
}
