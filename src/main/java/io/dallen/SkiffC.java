package io.dallen;

import io.dallen.compiler.visitor.ASTVisitor;
import io.dallen.compiler.CompileContext;
import io.dallen.compiler.CompiledCode;
import io.dallen.parser.Parser;
import io.dallen.tokenizer.Lexer;
import io.dallen.tokenizer.Token;

import java.io.IOException;
import java.io.PrintWriter;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;
import java.util.stream.Collectors;

public class SkiffC {

    public final static boolean DEBUG = true;

    private static void printTokenStream(List<Token> tokens) {
        tokens.forEach(e -> System.out.print(" " + e.toString()));
        System.out.println();
    }

    private static String readFile(String path) throws IOException {
        byte[] encoded = Files.readAllBytes(Paths.get(path));
        return new String(encoded, Charset.forName("UTF-8"));
    }

    private static String preamble = "#include \"lib/skiff.h\"\n\n";

    public static void main(String[] argz) throws IOException {
        String inFile = "test.skiff";
        String outfile = "test.c";
        String programText;
        try {
            programText = readFile(inFile);
        } catch(IOException err) {
            System.err.println("Bad file");
            return;
        }
        Lexer lexer = new Lexer(programText);
        List<Token> tokenStream = lexer.lex();
        printTokenStream(tokenStream);

        System.out.println(" ======== PARSE =========== ");

        Parser parser = new Parser(tokenStream);
        List<AST.Statement> statements = parser.parseBlock();
        statements.forEach(System.out::println);

        System.out.println(" ======== COMPILE =========== ");

        CompileContext context = new CompileContext(null);
        List<String> compiledText = statements
                .stream()
                .map(e -> e.compile(context))
                .map(CompiledCode::getCompiledText)
                .collect(Collectors.toList());

        String code = preamble + String.join("\n", compiledText);

        System.out.println(code);

        try (PrintWriter out = new PrintWriter(outfile)) {
            out.println(code);
        }
    }

    public static String compile(String code, CompileContext context) {
        Lexer lexer = new Lexer(code);
        List<Token> tokenStream = lexer.lex();
        Parser parser = new Parser(tokenStream);
        List<AST.Statement> statements = parser.parseBlock();
        return statements
                .stream()
                .map(e -> e.compile(context))
                .map(CompiledCode::getCompiledText)
                .collect(Collectors.joining("\n"));
    }
}
