<template>
  <div class="registers">
      <div v-for="(value, key) in registers_content" :key="key">
        <div class="register_key">{{ key }}</div>
        <div class="register_value">{{ value }}</div>
      </div>
  </div>
</template>

<script>
export default {
  name: "RegistersComponent",
  data() {
    return {
      play: true,
    };
  },
  props: {
    registers_content: Object,
  },
};
</script>

<style>
.registers {
  width: 100%;
}
.register_key {
  margin: 1px 1px 0px 0px;
  border-style: solid;
  display: inline-block;
  width: 30px;
}
.register_value {
  border-style: solid;
  display: inline-block;
  width: 100px;
}
button {
  align-items: center;
}
</style>
