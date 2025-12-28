import java.io.*;

class Calc {
    int token; int value; int ch;
    private PushbackInputStream input;
    final int NUMBER=256;

    Calc(PushbackInputStream is) {
        input = is;
    }

    int getToken( )  { /* tokens are characters */
        while(true) {
            try  {
	            ch = input.read();
                if (ch == ' ' || ch == '\t' || ch == '\r') ;
                else 
                    if (Character.isDigit(ch)) {
                        value = number( );
                        input.unread(ch);
                        return NUMBER;
                    }
                    else return ch;
            } catch (IOException e) {
                System.err.println(e);
            }
        }
    }

    private int number( )  {
    /* number -> digit { digit } */
        int result = ch - '0';
        try  {
            ch = input.read();
            while (Character.isDigit(ch)) {
                result = 10 * result + ch -'0';
                ch = input.read(); 
            }
        } catch (IOException e) {
            System.err.println(e);
        }
        return result;
    }

    void error( ) {
        System.out.printf("parse error : %d\n", ch);
        //System.exit(1);
    }

    void match(int c) { 
        if (token == c) 
        	token = getToken();
        else error();
    }

    void command( ) {
    /* command -> expr '\n' */
        // int result = aexp();
    	Object result = expr();
        if (token == '\n') /* end the parse and print the result */
	    System.out.println(result);
        else error();
    }
    
    Object expr() {
    /* <expr> -> <bexp> {& <bexp> | '|'<bexp>} | !<expr> | true | false */
    	Object result;
    	if (token == '!'){
    		match('!');
    		result = !(boolean) expr();
    	}
    	else if (token == 't'){
    		// TRUE
    		match('t');
    		result = (boolean)true;
    	}
    	else if (token == 'f'){
    		// FALSE
    		match('f');
    		result = (boolean)false;
    	}
    	else {
    		// &<bexp>|'|'<bexp>
    		result = bexp();
    		while (token == '&' || token == '|') {
    			if (token == '&'){
    	            match('&');
    	            Object result2 = bexp();
    	            result = (boolean)result && (boolean)result2;
    			}
    			else if (token == '|'){
    	            match('|');
    	            Object result2 = bexp();
    	            result = (boolean)result || (boolean)result2;
    			}
    		}
    	}
    	return result;
	}
    
    Object bexp() {
    /* <bexp> -> <aexp> [<relop> <aexp>] */
    	Object result;
    	int aexp1 = aexp();
    	if (token == '<' || token == '>' || token == '=' || token == '!'){ // <relop>
    		/* ( < | <= | > | >= | == | != ) */
			String relop_str = relop();
			int aexp2 = aexp();
			if (relop_str.equals("<")){
				result = (boolean)(aexp1 < aexp2); 
			}
			else if (relop_str.equals("<=")){
				result = (boolean)(aexp1 <= aexp2);
			}
			else if (relop_str.equals(">")){
				result = (boolean)(aexp1 > aexp2);
			}
			else if (relop_str.equals(">=")){
				result = (boolean)(aexp1 >= aexp2);
			}
			else if (relop_str.equals("==")){
				result = (boolean)(aexp1 == aexp2);
			}
			else if (relop_str.equals("!=")){
				result = (boolean)(aexp1 != aexp2);
			}
			else {result = ""; error();}
    	}
		else {
			result = aexp1;
		}    	
    	return result;
	}
        
    String relop() {    	
    /* <relop> -> ( < | <= | > | >= | == | != ) */    	
    	String result = "";
    	if (token == '<'){
    		match('<');
    		if (token == '='){
    			match('=');
    			result = "<=";
    		}
    		else {
    			result = "<";
    		}
    	}
    	else if (token == '>'){
    		match('>');
    		if (token == '='){
    			match('=');
    			result = ">=";
    		}
    		else {
    			result = ">";
    		}
    	}
    	else if (token == '='){
    		match('=');
    		match('=');
    		result = "==";    		
    	}
    	else if (token == '!'){
    		match('!');
    		match('=');
    		result = "!=";
    	}
    	else error();
    	
    	return result;
	}
        

    int aexp( ) {	
    /* <aexp> -> <term> { + <term> | - <term> } */
        int result = term();
        while (token == '+' || token == '-') {
        	if (token == '+'){
	            match('+');
	            result += term();
        	}
        	else {	// (token == '-')
        		match('-');
        		result -= term();
        	}
        }
        return result;
    }
    

    int term() {
    /* <term> -> <factor> { * <factor> | / <factor>} */
       int result = factor();
       while (token == '*' || token == '/') {
    	   if (token == '*') {
	           match('*');
	           result *= factor();
    	   }
    	   else {
	           match('/');
	           result /= factor();
    	   }
       }
       return result;
    }

    int factor() {
    /* factor -> '(' expr ')' | number */
        int result = 0;
        if (token == '(') {
            match('(');
            result = aexp();
            match(')');
        }
        else if (token == NUMBER) {
            result = value;
	        match(NUMBER); //token = getToken();
        }
        return result;
    }

    void parse( ) {
        token = getToken(); // get the first token
        command();          // call the parsing command
    }

    public static void main(String args[]) { 
        Calc calc = new Calc(new PushbackInputStream(System.in));
        while(true) {
            System.out.print(">> ");
            calc.parse();
        }
    }
}