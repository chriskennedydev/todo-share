# frozen_string_literal: true

if ARGV.length.zero?
  puts 'Need at least one argument!'
  exit 1
end

home = ENV['HOME']
todo_dir = File.join(home, '.todo')
todo_file = File.join(todo_dir, 'todo')
todo_num = 0

Dir.mkdir(todo_dir) unless Dir.exist?(todo_dir)

cmd = ARGV[0]
todo = ARGV[1..]

todo_num = ARGV[1] if %w[done update del].include?(cmd)
todo = ARGV[2..] if %w[done update del].include?(cmd)

case cmd
when 'add'
  f = File.open(todo_file, 'a')
  todo.each do |word|
    f.write "#{word} "
  end
  f.write "\n"
  f.close
when 'del'
  f = File.open(todo_file)
  todos = f.readlines.map(&:chomp)
  counter = 1

  f.close
  File.delete(todo_file)

  todos.each do |t|
    if todo_num.to_i != counter
      f = File.open(todo_file, 'a')
      f.write "#{t}\n"
      f.close
    end
    counter += 1
  end
when 'update'
  f = File.open(todo_file)
  todos = f.readlines.map(&:chomp)
  counter = 1

  f.close
  File.delete(todo_file)

  todos.each do |t|
    f = File.open(todo_file, 'a')
    if todo_num.to_i == counter
      todo.each do |word|
        f.write "#{word} "
      end
      f.write "\n"
    else
      f.write "#{t}\n"
    end
    counter += 1
    f.close
  end
when 'done'
  f = File.open(todo_file)
  todos = f.readlines.map(&:chomp)
  counter = 1

  f.close
  File.delete(todo_file)

  todos.each do |t|
    f = File.open(todo_file, 'a')
    if todo_num.to_i == counter
      f.write "#{t} ✓\n"
    else
      f.write "#{t}\n"
    end
    f.close
    counter += 1
  end
when 'list'
  f = File.open(todo_file)
  todos = f.readlines.map(&:chomp)
  counter = 1

  puts 'Todo List'
  puts '---------'
  todos.each do |t|
    if t.include? '✓'
      puts "\033[32m#{counter}. #{t}\033[0m"
    else
      puts "#{counter}. #{t}"
    end
    counter += 1
  end
  f.close
else
  puts 'Invalid option'
end
